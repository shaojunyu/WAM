#ifndef FILE_H_
#define FILE_H_

/* some file processing related functions */
#include <stdio.h>
#include <stdlib.h>
#include "sequence.h"
#include "queue.h"

file_to_short_seq(FILE *fp, Short_seq *front_short_seq, Short_seq *backgroud_short_seq)
{
    //reading files
    if (fp != NULL)
    {
        char line[400];  // a line of text
        memset(line, '\0', 400 * sizeof(char));
        fgets(line, 400, fp);
        fgets(line, 400, fp); //get the second line of file
        regex_t r;  //regex struct
        regmatch_t match_ptr[200];  //store matched info
        memset(match_ptr, '\0', 200 * sizeof(regmatch_t)); //栈内存，使用前置空
        regcomp(&r, "[0-9]+", REG_EXTENDED);  //compile before use regex struct
        int i = 0, j = 0;
        while (regexec(&r, &line[j], 1, &match_ptr[i], 0) == 0)  //match once a time
        {
            //printf("%d\n", match_ptr[i].rm_eo);
            j += match_ptr[i].rm_eo;
            i++;
        }

        int offset = 0;
        for (i = 0; match_ptr[i].rm_eo > 0 ; ++i)
        {
            match_ptr[i].rm_so += offset;
            match_ptr[i].rm_eo += offset;
            offset = match_ptr[i].rm_eo;
        }

        //get position data
        int position[200];
        memset(position, '\0', 200 * sizeof(int));
        for (i = 0; match_ptr[i].rm_eo > 0 ; ++i)
        {
            char buff[10];
            memset (buff, '\0', 10 * sizeof(char));
            strncpy(buff, &line[match_ptr[i].rm_so], match_ptr[i].rm_eo - match_ptr[i].rm_so);
            buff[match_ptr[i].rm_eo - match_ptr[i].rm_so] = '\0';   /* null character manually added */
            position[i] = atoi(buff);  //convert string to int
        }

        //get sequence
        char buff[100];
        memset(buff, '\0', 100 * sizeof(char));
        char seq[512000];  //sequence
        memset(seq, '\0', 512000);
        char *line_break = NULL;
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

        //get front_short_seq
        for (i = 0; position[i] > 0 ; ++i)
        {
            int start = position[i] - 4;
            if (start > 0)
            {
                Short_seq *ss = (Short_seq * )malloc(sizeof(Short_seq));
                memcpy(ss->seq, &seq[start], 9);
                QUEUE_INSERT_HEAD(&(front_short_seq->queue), &(ss->queue));
                //short_seq->seq[9] = '\0';
            }
        }

        if (backgroud_short_seq != NULL)
        {
            //get front_short_seq
            for (i = 0; i < strlen(seq) / 9; ++i)
            {
                Short_seq *ss = (Short_seq * )malloc(sizeof(Short_seq));
                memcpy(ss->seq, &seq[i * 9], 9);
                QUEUE_INSERT_HEAD(&(backgroud_short_seq->queue), &(ss->queue));
            }
        }

    }
}
#endif