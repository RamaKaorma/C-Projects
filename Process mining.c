/* Program to discover a process model from an event log.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  August 2022, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: RAMA KAORMA 1353255
  Dated:     30 September 2022

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define NEWLN '\n'
#define COMMA ','

#define PAIR 2
#define FULL -100
#define EMPTY -1
#define INIT_MAX 100
#define SUCCESSFUL 2147483647 /*Set maximum possible number to mean success*/

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef unsigned int action_t;  // an action is identified by an integer

typedef struct event event_t;   // an event ...
struct event {                  // ... is composed of ...
    action_t actn;              // ... an action that triggered it and ...
    event_t* next;              // ... a pointer to the next event in the trace
};

typedef struct {                // a trace is a linked list of events
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {                // an event log is an array of distinct traces
                                //     sorted lexicographically
    trace_t* trcs;              // an array of traces
    int      ndtr;              // the number of distinct traces in this log
    int      free;              // the next free value in the array log
    int      cpct;              // the capacity of this event log as the number
                                //     of  distinct traces it can hold
} log_t;

typedef action_t** DF_t;        // a directly follows relation over actions
struct event *head = NULL;
typedef int freq_tracker_t[INIT_MAX][PAIR];

/* FUNCTION PROTOTYPES -------------------------------------------------------*/
void init_trace(trace_t *trace);                /*Initialize a trace*/

trace_t *add_event(trace_t *trace, char value);      /*Add event to trace*/

char remove_event(trace_t *trace);
    /*Remove event from trace; call repeatedly to clear trace*/

void create_trace();
    /*Create a new trace in the log*/

void get_trace();
    /*create arr of distinct characters and their frequency in input*/

int search_for_event(char value, int arr[][PAIR], int count);
    /*Search for event in array of distinct events*/

int search_for_trace(log_t *log, trace_t *trace, int count);
    /*Search for trace in the trcs array in log*/

int count_distinct_events(freq_tracker_t arr);
    /*Count number of events in arr*/

int most_freq_trace(trace_t *trace, log_t *log);
    /*Find the frequency of the most frequent trace*/

void find_sequence();
    /*Find sequences in arr and print their frequencies*/

void sort_2D_array();
    /*Sort a 2D array*/

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {

    log_t *log;                     /*Pointer to our log*/
    trace_t *trace;                 /*Pointer to a single trace*/
    freq_tracker_t dist_events;     /*Array of distinct events*/

    int tot_trace_count = 0;                /*Number of traces in total*/
    int tot_event_count = 0;                /*Number of events in total*/
    int dist_event_count = 0;               /*Number of distinct events*/


    /*Read 1 trace at a time*/
    int c;
    while ((c=getchar()) != EOF) {
        get_trace(c, &dist_event_count, &tot_event_count,
                  &tot_trace_count, &dist_events, &trace, &log);
    }

    /*Search for trace with highest frequency*/
    int highest_tr_freq = most_freq_trace(trace, log);

    /*STAGE 0*/
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", dist_event_count);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", tot_event_count);
    printf("Total number of traces: %d\n", tot_trace_count);
    printf("Most frequent trace frequency: %d\n", highest_tr_freq);

    /*Print the most frequent trace*/

    /*Sort the array of distinct events*/
    freq_tracker_t *p_dist_events = &dist_events;
    sort_2D_array(p_dist_events, dist_event_count);

    /* Now print each event with its frequency */
    for (int i = 0; i<dist_event_count; i++) {
        printf("%c = %d\n", dist_events[i][0], dist_events[i][1]);
    }


    /*STAGE 1*/
    freq_tracker_t seq;
    for (int i=0; i<log->ndtr; i++) {
        find_sequence(&log->trcs[i], seq);
    }

    /*Free memory*/
    for (int i=0; i<log->ndtr; i++) {
        while ((log->trcs[i].head->next) != NULL) {
            remove_event(&log->trcs[i]);
        }
    }

    return EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/
void
init_trace(trace_t *trace) {
    /*Initialize head and foot*/
    trace->head = NULL;
    trace->foot = NULL;
}


/*----------------------------------------------------------------------------*/
trace_t
*add_event(trace_t *trace, char value) {
    /* Alistair Moffat, PPSAA, Chapter 10, December 2012
     * (c) University of Melbourne
     * Date copied and modified: 14 Oct 2022 */
     /*Create space for new event in memory*/
    event_t *node = (event_t *) malloc(sizeof(*node));

    /*Check that the trace exist and space in memory is available*/
    assert(trace != NULL && node != NULL);

    /*Set up new node*/
    node->actn = value;
    node->next = NULL;

    if (trace->foot != NULL) {
        /*List is empty, then head = foot*/
        trace->head = node;
        trace->foot = node;
    } else {
        /*List already has nodes*/
        trace->foot->next = node;
        trace->foot = node;
    }

    return trace;
}


/*----------------------------------------------------------------------------*/
char
remove_event(trace_t *trace) {
    /* CODE WRITTEN WITH REFERENCE TO:
    * Jacob Sorber
    * https://www.youtube.com/watch?v=FcIubL92gaI
    * Date copied and modified: October 6, 2022
    */

    /*Trace is empty already*/
    if (trace->head == NULL) {
        return EMPTY;
    }
    /*temporary pointer to head of trace*/
    event_t *tmp = trace->head;

    /*Value to hold the event*/
    char value = tmp->actn;

    /*Direct pointer onto the next event in the list*/
    trace->head = trace->head->next;

    /*If new head is equal to null, then it's end of list,
        set the foot to null as well*/
    if (trace->head == NULL) {
        trace->foot = NULL;
    }

    /*returning the element that was deleted*/
    return value;
}


/*----------------------------------------------------------------------------*/
void
create_trace(log_t *log, freq_tracker_t traces, trace_t *trace) {
    event_t *start = trace->head; /*Assign head*/
    init_trace(trace);
    event_t *end = NULL;
    int guard = 0;

    log->trcs->head[guard] = *start;

    /*Get to the foot of the trace*/
    while (trace->foot->next != NULL) {
        end = trace->foot;
    }
    /*Assign foot*/
    trace->foot = end;

    /*Add trace to array of traces in log*/
    log->trcs[log->free] = *trace;
}


/*----------------------------------------------------------------------------*/
void
get_trace(int c, int *dist_event_count, int *tot_event_count,
          int *tot_trace_count, freq_tracker_t events,
          trace_t *trace, log_t *log) {

    /*Define our variables and new trace*/
    int found;
    trace_t *added = (trace_t *)malloc(sizeof(*added));
    init_trace(added);

    if (c != COMMA && c != NEWLN) {
        /*Found an event*/
        (*tot_event_count)++;
        found = search_for_event(c, events, (*dist_event_count));

        if (found != EMPTY) {
            /*Event is not distinct, only increase its frequency*/
            events[found][1]++;

        } else {
            /*Event is distinct, add it to array of events*/
            events[(*dist_event_count)][0] = (int) c;
            events[(*dist_event_count)][1] = 1;
            (*dist_event_count)++;

            /*Add event to the trace, which is a linked list*/
            added = add_event(trace, c);

        }
    } else if (c == NEWLN) {
        /*End of trace*/
        event_t *temporary = trace->head;
        while (temporary != NULL) {
            temporary = temporary->next;
        }

        found = search_for_trace(log, trace, log->ndtr);
        if (found != EMPTY) {
            /*Trace not distinct*/
            log->trcs[found].freq++;
        } else {
            /*Trace is distinct*/
            log->trcs[log->ndtr] = *trace;
            log->ndtr++;
        }

        /*Tie the loose end*/
        trace->foot = NULL;
        return;

    } else {
        return;
    }


}


/*----------------------------------------------------------------------------*/
int
search_for_event(char value, int arr[][PAIR], int count) {
    /*Search for event in array of distinct events
    Recall that arr has format { {actn1, freq1} {actn2, freq2} } */
    for (int i = 0; i<count; i++) {
        if (arr[i][0] == (int) value) {
            return i;
        }
    }
    /*Not found*/
    return EMPTY;
}


/*----------------------------------------------------------------------------*/
int
search_for_trace(log_t *log, trace_t *trace, int count) {
    /*Empty array*/
    if (log->trcs[0].head == NULL) {
        return EMPTY;
    }

    for (int i=0; i<count; i++) {
        /*Create space in memory to copy the a trace to compare against*/
        trace_t *cur_trace = (trace_t *)malloc(sizeof(cur_trace));
        cur_trace = &log->trcs[i];
        while (trace->head->next != NULL && cur_trace->head->next != NULL) {
            /*Each every 2 nodes together*/
            if (trace->head != cur_trace->head) {
                /*No match*/
                break;
            } else {
                /*Match found, change to the next two nodes*/
                trace->head = trace->head->next;
                cur_trace->head = cur_trace->head->next;
            }
            /*Reached the end of one linked list*/
            if (trace->head != NULL || cur_trace->head != NULL) {
                return EMPTY;
            }
            return i;
        }
    }

    return EMPTY;
}


/*----------------------------------------------------------------------------*/
int
most_freq_trace(trace_t *trace, log_t *log) {
    int highest_freq = 0;
    for (int i=0; i < log->ndtr; i++) {
        /*check every trace in trcs array in the log*/
        if (log->trcs[i].freq > highest_freq) {
            highest_freq = log->trcs[i].freq;
        }
    }
    return highest_freq;
}


/*----------------------------------------------------------------------------*/
void
sort_2D_array(freq_tracker_t *arr, int count) {

    int tmp[1][PAIR] = {{0, 0}}; /*Help switch the event-frequency arrays*/

    for (int i=0; i<count-1; i++) {
        for (int j=1; j<count; j++) {
            if (arr[j][0] < arr[i][0]) {
                *tmp[0] = *arr[i][0];
                *tmp[1] = *arr[i][1];
                *arr[i][0] = *arr[j][0];
                *arr[i][1] = *arr[j][1];
                *arr[j][0] = *tmp[0];
                *arr[j][1] = *tmp[1];
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void
find_sequence(trace_t *trace, freq_tracker_t seq) {


    return;
}


/*Algorithms are fun!*/
