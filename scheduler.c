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
};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

/*Function to sort linked list by arrival time*/
void sort_by_arrival(struct job **head_ptr)
{
  struct job *current, *next;
  int swapped;

  do
  {
    swapped = 0;
    current = *head_ptr;

    while (current->next != NULL)
    {
      next = current->next;
      if (current->arrival > next->arrival)
      {
        // Swap data
        int temp_id = current->id;
        int temp_arrival = current->arrival;
        int temp_length = current->length;
        int temp_tickets = current->tickets;

        current->id = next->id;
        current->arrival = next->arrival;
        current->length = next->length;
        current->tickets = next->tickets;

        next->id = temp_id;
        next->arrival = temp_arrival;
        next->length = temp_length;
        next->tickets = temp_tickets;

        swapped = 1;
      }
      current = current->next;
    }
  } while (swapped);
}

/*Function to sort the linked list based on length*/
struct job *sort_by_length(struct job *head)
{
  struct job *new_head = NULL;
  struct job *current, *next;
  int swapped;

  do
  {
    swapped = 0;
    current = head;

    while (current->next != NULL)
    {
      next = current->next;
      if (current->length > next->length)
      {
        // Swap data
        int temp_id = current->id;
        int temp_arrival = current->arrival;
        int temp_length = current->length;
        int temp_tickets = current->tickets;

        current->id = next->id;
        current->arrival = next->arrival;
        current->length = next->length;
        current->tickets = next->tickets;

        next->id = temp_id;
        next->arrival = temp_arrival;
        next->length = temp_length;
        next->tickets = temp_tickets;

        swapped = 1;
      }
      current = current->next;
    }
  } while (swapped);

  // Set the new head pointer
  new_head = head;

  return new_head;
}

/*Function to copy linked list*/
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
  tmp->endTime = -1;

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

/*Function to remove a node in the linked list*/
void remove_node(struct job **head_ptr, int target_id)
{
  struct job *current = *head_ptr;
  struct job *prev = NULL;

  // Case: Node to be removed is the head
  if (current != NULL && current->id == target_id)
  {
    *head_ptr = current->next;
    free(current);
    return;
  }

  // Search for the node to be removed
  while (current != NULL && current->id != target_id)
  {
    prev = current;
    current = current->next;
  }

  // If node was not found
  if (current == NULL)
  {
    printf("Node with ID %d not found.\n", target_id);
    return;
  }

  // Case: Node to be removed is in the middle or the end
  prev->next = current->next;
  free(current);
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

      if (shortest->startTime == -1)
      {
        shortest->startTime = current_time;
      }
      // Run JL for S ticks
      int run_time = (shortest->length < slice) ? shortest->length : slice;

      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n",
             current_time, shortest->id, shortest->arrival, run_time);

      shortest->length -= run_time;
      current_time += run_time;

      // Check if the job is complete
      if (shortest->length == 0)
      {
        shortest->endTime = current_time;
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
  struct job *copy = copy_linked_list(head);

  int total_response = 0;
  int total_turnaround = 0;
  int total_wait = 0;
  int num_jobs = get_node_count(copy);

  while (copy != NULL)
  {
    int response_time = copy->startTime - copy->arrival;
    int turnaround_time = copy->endTime - copy->arrival;
    int wait_time = response_time; // Since arrival time is 0

    total_response += response_time;
    total_turnaround += turnaround_time;
    total_wait += wait_time;

    printf("%d", response_time);
    printf("%d", turnaround_time);
    printf("%d", wait_time);
    printf("%d", copy->id);

    printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
           copy->id, response_time, turnaround_time, wait_time);

    copy = copy->next;
  }

  double avg_response = (double)total_response / num_jobs;
  double avg_turnaround = (double)total_turnaround / num_jobs;
  double avg_wait = (double)total_wait / num_jobs;

  printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
         avg_response, avg_turnaround, avg_wait);
}

void policy_RR(struct job *head, int slice)
{
  // TODO: Fill this in

  return;
}

void analyze_RR(struct job *head)
{
  // TODO: Fill this in

  return;
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
      printf("Begin analyzing STCF:\n");
      analyze_STCF(head);
      printf("End analyzing STCF.\n");
    }

    exit(EXIT_SUCCESS);
  }
  if (strcmp(policy, "RR") == 0)
  {
    policy_RR(head, slice);
    if (analysis)
    {
      printf("Begin analyzing RR:\n");
      analyze_RR(head);
      printf("End analyzing RR.\n");
    }

    exit(EXIT_SUCCESS);
  }
  if (strcmp(policy, "LT") == 0)
  {
    policy_LT(head, slice);
    if (analysis)
    {
      printf("Begin analyzing STCF:\n");
      analyze_LT(head);
      printf("End analyzing STCF.\n");
    }

    exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies

  exit(EXIT_SUCCESS);
}
