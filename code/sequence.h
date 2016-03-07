#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "queue.h"
#include <stdlib.h>
#include "jwHash.h"

typedef struct Short_seq
{
    Queue queue;
    char seq[9];
} Short_seq;

void SS_INIT(Short_seq *short_seq)
{
    QUEUE_INIT(&(short_seq->queue));
}

void SS_FREE(Short_seq *short_seq)
{

}

void caculate_p_table(jwHashTable *f_table, Short_seq *short_seq, int *num)
{
    Queue *tmp = short_seq->queue.next;
    int sum = -1;
    while (tmp != &(short_seq->queue))
    {
        Short_seq *ss = (Short_seq *)tmp;
        char key[4];
        double value;

        int i;
        for (i = 0; i < 9; ++i)
        {

            if (i == 0)
            {
                key[0] = '1';
                key[1] = ss->seq[i];
                key[2] = '\0';
                if (get_dbl_by_str(f_table, (char *)&key, &value) == HASHNOTFOUND)
                {
                    add_dbl_by_str(f_table, (char *)&key, 1);
                }
                else
                {
                    add_dbl_by_str(f_table, (char *)&key, value + 1);
                }
            }
            else
            {
                sprintf(&key[0], "%d", (i + 1));
                key[1] = ss->seq[i - 1];
                key[2] = ss->seq[i];
                key[3] = '\0';

                if (get_dbl_by_str(f_table, (char *)&key, &value) == HASHNOTFOUND && strlen(key) == 3)
                {
                    add_dbl_by_str(f_table, (char *)&key, 1);
                }
                else
                {
                    add_dbl_by_str(f_table, (char *)&key, value + 1);
                }
            }
        }
        tmp = tmp->next;
        sum++;
    }
    *num = sum;
}

void predict_seq(FILE *fp, double score, Short_seq *predicted_seq, jwHashTable *front_hashTable, int front_short_seq_num, jwHashTable *background_hashTable, int backgroud_short_seq_num)
{
    //get sequence
    char buff[100];
    memset(buff, '\0', 100 * sizeof(char));
    char seq[512000];  //sequence
    memset(seq, '\0', 512000);
    char *line_break = NULL;
    fgets(buff, 400, fp);
    fgets(buff, 400, fp);
    while (fgets(buff, sizeof(buff), fp))
    {
        //remove the line break char
        line_break = strchr(buff, '\r');
        if (line_break)
        {
            *line_break = '\0';
        }
        strcat(seq, buff);
    }

    int i;
    for (i = 0; i < (int)strlen(seq) - 9; ++i)
    {
        double s = 0;
        double front_p;
        double background_p;
        char key[4];
        int j;
        for(j = 0; j < 9; j++)
        {
            if (j == 0)
            {
                key[0] = '1';
                key[1] = seq[i] + 0x20;
                key[2] = '\0';
                front_p = (get_dbl_by_str(front_hashTable, (char *)&key, &front_p) == HASHNOTFOUND) ? 0.25 * front_short_seq_num : front_p;
                front_p = front_p / front_short_seq_num;
                background_p = (get_dbl_by_str(background_hashTable, (char *)&key, &background_p) == HASHNOTFOUND) ? 0.25 * backgroud_short_seq_num : background_p;
                background_p = background_p / backgroud_short_seq_num;
            }
            else
            {
                sprintf(&key[0], "%d", (j + 1));
                key[1] = seq[i + j - 1] + 0x20;
                key[2] = seq[i + j] + 0x20;
                key[3] = '\0';
                front_p = (get_dbl_by_str(front_hashTable, (char *)&key, &front_p) == HASHNOTFOUND) ? 0.0625 * front_short_seq_num : front_p;
                front_p = front_p / front_short_seq_num;
                background_p = (get_dbl_by_str(background_hashTable, (char *)&key, &background_p) == HASHNOTFOUND) ? 0.0625 * backgroud_short_seq_num : background_p;
                background_p = background_p / backgroud_short_seq_num;
            }
            //printf("%f   %f\n",front_p,background_p);
            s += log(front_p) / log(background_p);
        }
        if (s > score)
        {
            Short_seq *ss = (Short_seq * )malloc(sizeof(Short_seq));
            memcpy(ss->seq, &seq[i], 9);
            QUEUE_INSERT_HEAD(&(predicted_seq->queue), &(ss->queue));
        }
    }
}

#endif
