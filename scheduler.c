#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>


// TODO: Add more fields to this struct
struct job
{
  int id;
  int arrival;
  int length;
  int tickets;
  struct job *next;
  
  //RR
  int startTimes;
  int endTime;
  int originalLength; 

  //Stcf
  int startTime;
  int completionTime;
  
  //LT
  int start_time;
  int finish_time;
  int time_run;

};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/



void append(int id, int arrival, int length, int tickets)
{
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job *)malloc(sizeof(struct job));

  // tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->tickets = tickets;

  //RR
  tmp->startTimes = -1;
  tmp->originalLength = length; 

  //STCF
  tmp->startTime = -1;
  tmp->completionTime = -1;

  //LT
  tmp->start_time = -1;
  tmp->finish_time = -1;
  tmp->time_run = 0;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty
  if (head == NULL)
  {
    head = tmp;
    return;
  }

  struct job *prev = head;

  // Find end of list
  while (prev->next != NULL)
  {
    prev = prev->next;
  }

  // Add job to end of list
  prev->next = tmp;
  return;
}



void read_workload_file(char *filename)
{
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL,
       *length = NULL;
  int tickets = 0;

  struct job **head_ptr = malloc(sizeof(struct job *));

  if ((fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1)
  {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
    tickets += 100;


    


    // Make sure neither arrival nor length are null.
    assert(arrival != NULL && length != NULL);

    append(id++, atoi(arrival), atoi(length), tickets);
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}

// New functions


void sort_by_arrival() {
    struct job *current, *next;
    int swapped;

    do {
        swapped = 0;
        current = head;

        while (current != NULL && current->next != NULL) {
            next = current->next;
            if (current->arrival > next->arrival) {
                // Swap the jobs
                int temp_id = current->id;
                int temp_arrival = current->arrival;
                int temp_length = current->length;

                current->id = next->id;
                current->arrival = next->arrival;
                current->length = next->length;

                next->id = temp_id;
                next->arrival = temp_arrival;
                next->length = temp_length;

                swapped = 1;  // mark that a swap occurred
            }
            current = current->next;
        }
    } while (swapped);
}


struct job *copy_linked_list(struct job *original)
{
  struct job *new_head = NULL;
  struct job *tail = NULL;

  while (original != NULL)
  {
    struct job *new_node = (struct job *)malloc(sizeof(struct job));
    new_node->id = original->id;
    new_node->arrival = original->arrival;
    new_node->length = original->length;
    new_node->tickets = original->tickets;
    new_node->next = NULL;

    if (new_head == NULL)
    {
      new_head = new_node;
      tail = new_node;
    }
    else
    {
      tail->next = new_node;
      tail = new_node;
    }

    original = original->next;
  }

  return new_head;
}



int get_node_count(struct job *head)
{
  int count = 0;
  struct job *current = head;

  while (current != NULL)
  {
    count++;
    current = current->next;
  }

  return count;
}



void policy_STCF(struct job *head, int slice)
{
  struct job *copy = copy_linked_list(head);
  int current_time = 0;
  int num_jobs = get_node_count(copy);

  printf("Execution trace with STCF:\n");

  while (num_jobs != 0)
  {
    struct job *current = copy;
    struct job *shortest = NULL;

    // Find the job with the shortest remaining duration
    while (current != NULL)
    {
      if (current->arrival <= current_time && current->length > 0)
      {
        if (shortest == NULL || current->length < shortest->length)
        {
          shortest = current;
        }
      }
      current = current->next;
    }

    if (shortest == NULL)
    {
      // No jobs are ready to run at this time
      current_time++;
    }
    else
    {
      // Run JL for S ticks
      int run_time = (shortest->length < slice) ? shortest->length : slice;

      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n",
             current_time, shortest->id, shortest->arrival, run_time);

      // Set startime for each job

      struct job *tmp = head;
      while (tmp != NULL)
      {
        if (tmp->id == shortest->id)
        {
          if (tmp->startTime == -1)
          {
            tmp->startTime = current_time;
          }
          break; // Break regardless of whether startTime was set or not
        }
        tmp = tmp->next;
      }

      shortest->length -= run_time;
      current_time += run_time;

      // Check if the job is complete
      if (shortest->length == 0)
      {
        tmp = head;
        while (tmp != NULL)
        {
          if (tmp->id == shortest->id)
          {
            tmp->completionTime = current_time;
            break;
          }
          tmp = tmp->next;
        }
        struct job *temp = shortest;
        current = shortest->next;
        free(temp);
        num_jobs--;
      }
    }
  }

  printf("End of execution with STCF.\n");
  
}

void analyze_STCF(struct job *head)
{
  int total_response = 0;
  int total_turnaround = 0;
  int total_wait = 0;
  int num_jobs = get_node_count(head);

  while (head != NULL)
  {
    int response_time = head->startTime - head->arrival;
    int turnaround_time = head->completionTime - head->arrival;
    int wait_time = turnaround_time - head->length;

    total_response += response_time;
    total_turnaround += turnaround_time;
    total_wait += wait_time;

    printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
           head->id, response_time, turnaround_time, wait_time);

    head = head->next;
  }

  double avg_response = (double)total_response / num_jobs;
  double avg_turnaround = (double)total_turnaround / num_jobs;
  double avg_wait = (double)total_wait / num_jobs;

  printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
         avg_response, avg_turnaround, avg_wait);
}


void policy_RR(struct job *head, int slice) {


  sort_by_arrival(); 
  int currentTime = 0;
  int total_jobs = 0; //finding total jobs
  int job_executed = 0;
  int jobs_ran_this_round;

  struct job *job_counter = head;
  while (job_counter) {
    total_jobs++;
    job_counter = job_counter->next;
  }
  
  printf("\nStart of execution with RR.\n\n");

  struct job *current = head; //current pointer points to head
  while (job_executed < total_jobs)  // Keep looping till no jobs are left to be executed
  {
    current = head;
    jobs_ran_this_round = 0;

    while (current != NULL)
    {
      if (current->arrival <= currentTime && current->length != 0)
      {
        
        jobs_ran_this_round++;
        int time_ran = 0;
        if(current->length > slice){
          time_ran = slice;   
          current->length -= time_ran;
        }
        else{
          time_ran = current->length;
          current->length = 0;
          job_executed++;
        }

        
        printf("  t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", currentTime, current->id, current->arrival, time_ran);
        
        if (current->startTimes == -1) {
          current->startTimes = currentTime;
        }
        
        currentTime += time_ran;

        if (current->length == 0) {
            current->endTime = currentTime;
        }
        
      }
      current = current->next;
    }

    if (jobs_ran_this_round == 0) { // if no jobs ran in this round, advance the clock
      struct job *temp = head;
      while (temp != NULL && temp->length == 0) { // find the next job that hasn't started yet
        temp = temp->next;
      }
      if (temp != NULL) { // if found, set currentTime to its arrival time
        currentTime = temp->arrival;
      }
    }
  }
  printf("\nEnd of execution with RR.\n");
  return;

}

void analyze_RR(struct job *head)
{
  struct job *current = head;
    double total_response = 0, total_turnaround = 0, total_wait = 0;
    int count = 0;

    while (current != NULL)
    {
        // Response time: time when job first gets the CPU - arrival time
        int response_time = current->startTimes - current->arrival;

        // Turnaround time: completion time - arrival time
        int turnaround_time = current->endTime - current->arrival;

        // Wait time: Turnaround time - total execution time
        int wait_time = turnaround_time - current->originalLength;

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               current->id, response_time, turnaround_time, wait_time);

        total_response += response_time;
        total_turnaround += turnaround_time;
        total_wait += wait_time;
        count++;
        current = current->next;
    }

    // Compute average metrics
    double avg_response = total_response / count;
    double avg_turnaround = total_turnaround / count;
    double avg_wait = total_wait / count;

    printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", avg_response, avg_turnaround, avg_wait);
    
}

struct job *lottery_select(struct job *head, int current_time) {
    int total_tickets = 0;

    // Count the total number of tickets for jobs that have arrived
    struct job *current = head;
    while (current != NULL) {
        if (current->arrival <= current_time && current->length > 0) {
            total_tickets += current->tickets;
        }
        current = current->next;
    }

    // If no tickets, return NULL
    if (total_tickets == 0) {
        return NULL;
    }

    // Select a random ticket
    int winning_ticket = rand() % total_tickets;

    // Find which job the winning ticket belongs to
    current = head;
    int ticket_count = 0;
    while (current != NULL) {
        if (current->arrival <= current_time && current->length > 0) {
            ticket_count += current->tickets;
            if (winning_ticket < ticket_count) {
                return current;
            }
        }
        current = current->next;
    }
    return NULL; // This line should never be reached.
}

void policy_LT(struct job *head, int slice) {
    int current_time = 0;
    int total_jobs = get_node_count(head);
    int job_executed = 0;

    printf("Execution trace with LT:\n");

    while (job_executed < total_jobs) {
        struct job *selected = lottery_select(head, current_time);

        if (selected == NULL) {
            // No jobs are ready to run at this time
            current_time++;
            continue;
        }

        int run_time = (selected->length < slice) ? selected->length : slice;
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", current_time, selected->id, selected->arrival, run_time);

        // Update job parameters
        if (selected->start_time == -1) {
            selected->start_time = current_time;
        }

        selected->length -= run_time;
        selected->time_run += run_time;
        current_time += run_time;

        if (selected->length == 0) {
            selected->finish_time = current_time;
            job_executed++;
        }
    }

    printf("End of execution with LT.\n");
}

void analyze_LT(struct job *head)
{
    struct job *current = head;
    double total_response = 0, total_turnaround = 0, total_wait = 0;
    int count = 0;

    while (current != NULL)
    {
        // Response time: time when job first gets the CPU - arrival time
        int response_time = current->start_time - current->arrival;

        // Turnaround time: completion time (finish_time) - arrival time
        int turnaround_time = current->finish_time - current->arrival;

        // Wait time: Turnaround time - total execution time (original length)
        int wait_time = turnaround_time - current->time_run;

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               current->id, response_time, turnaround_time, wait_time);

        total_response += response_time;
        total_turnaround += turnaround_time;
        total_wait += wait_time;
        count++;
        current = current->next;
    }

    // Compute average metrics
    double avg_response = total_response / count;
    double avg_turnaround = total_turnaround / count;
    double avg_wait = total_wait / count;

    printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", avg_response, avg_turnaround, avg_wait);
}


int main(int argc, char **argv)
{

  srand(time(NULL));

  if (argc < 5)
  {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file slice-length\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];
  int slice = atoi(argv[4]);

  // Note: we use a global variable to point to
  // the start of a linked-list of jobs, i.e., the job list
  read_workload_file(workload);
  

  if (strcmp(policy, "STCF") == 0)
  {
    policy_STCF(head, slice);
    if (analysis)
    {
      printf("Begin analyzing STCF:\n\n");
      analyze_STCF(head);
      printf("\nEnd analyzing STCF.\n");
    }

    exit(EXIT_SUCCESS);
  }


  if (strcmp(policy, "RR") == 0)
  {
    policy_RR(head, slice);
    if (analysis)
    {
      printf("Begin analyzing RR:\n\n");
      analyze_RR(head);
      printf("\nEnd analyzing RR.\n");
    }

    exit(EXIT_SUCCESS);
  }
  if (strcmp(policy, "LT") == 0)
  {
    policy_LT(head, slice);
    if (analysis)
    {
      printf("Begin analyzing LT:\n\n");
      analyze_LT(head);
      printf("\nEnd analyzing LT.\n");
    }

    exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies

  exit(EXIT_SUCCESS);
}