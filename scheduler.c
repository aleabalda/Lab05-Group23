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
  

  int startTime;
  int endTime;

  int originalLength; // New field
  int executed;

};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/



/*Function to append a new job to the list*/
void append(int id, int arrival, int length, int tickets)
{
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job *)malloc(sizeof(struct job));

  // tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->tickets = tickets;

  tmp->startTime = -1;
  //tmp->endTime = -1;
  tmp->executed = 0;
  tmp->originalLength = length; 


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


/*Function to read in the workload file and create job list*/
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


void policy_STCF(struct job *head, int slice)
{
  
 
}

void analyze_STCF(struct job *head)
{
  
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
        
        if (current->startTime == -1) {
          current->startTime = currentTime;
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
        int response_time = current->startTime - current->arrival;

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


void policy_LT(struct job *head, int slice)
{
  // TODO: Fill this in

  return;
}

void analyze_LT(struct job *head)
{
  // TODO: Fill this in

  return;
}

int main(int argc, char **argv)
{

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
      printf("Begin analyzing STCF:\n\n");
      analyze_LT(head);
      printf("\nEnd analyzing STCF.\n");
    }

    exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies

  exit(EXIT_SUCCESS);
}
