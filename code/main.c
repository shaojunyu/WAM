#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include <math.h>
#include "file.h"
#include "sequence.h"
#include "jwHash.h"

int main(int argc, char const *argv[])
{

    //获取训练集数据
    printf("%s\n", "--------------------------------");
    DIR *dp;  //declare the dir pointer
    struct dirent *ep; //declare the dir enter pointer
    FILE *fp; //declare the file pointer
    char path[100];  //path to txt files
    getcwd(path, sizeof(path));
    strcat(path, "/Training/");

    //data
    Short_seq front_short_seq;
    Short_seq backgroud_short_seq;
    SS_INIT(&front_short_seq);
    SS_INIT(&backgroud_short_seq);

    dp = opendir(path);
    if (dp != NULL) //reading files
    {
        while(ep = readdir(dp))
        {
            if (!strcmp(ep->d_name, "."))
                continue;
            if (!strcmp(ep->d_name, ".."))
                continue;

            char file_path[100];// full file path with filename
            strcpy(file_path, path);
            strcat(file_path, ep->d_name);
            fp = fopen(file_path, "r");
            //puts(ep->d_name);
            file_to_short_seq(fp, &front_short_seq, &backgroud_short_seq);  //function in file.h
            fclose(fp);
        } //end while
    }
    else
    {
        printf("%s\n", "no files");
    }


    //数据统计
    jwHashTable *front_hashTable;    //前景概率front
    jwHashTable *background_hashTable;    //背景概率background
    front_hashTable = create_hash(150);
    background_hashTable = create_hash(150);
    int front_short_seq_num;
    int backgroud_short_seq_num;
    caculate_p_table(front_hashTable, &front_short_seq, &front_short_seq_num);
    caculate_p_table(background_hashTable, &backgroud_short_seq, &backgroud_short_seq_num);

    //对training数据打分
    double score = 100;

    Queue *tmp = front_short_seq.queue.next;
    while(tmp != &(front_short_seq.queue))
    {
        Short_seq *ss = (Short_seq *)tmp;
        double s = 0;
        double front_p;
        double background_p;
        char key[4];

        int i;
        for (i = 0; i < 9; ++i)
        {
            if (i == 0)
            {
                key[0] = '1';
                key[1] = ss->seq[0];
                key[2] = '\0';
                front_p = (get_dbl_by_str(front_hashTable, (char *)&key, &front_p) == HASHNOTFOUND) ? 0.25 * front_short_seq_num : front_p;
                front_p = front_p / front_short_seq_num;
                background_p = (get_dbl_by_str(background_hashTable, (char *)&key, &background_p) == HASHNOTFOUND) ? 0.25 * backgroud_short_seq_num : background_p;
                background_p = background_p / backgroud_short_seq_num;
            }
            else
            {
                sprintf(&key[0], "%d", (i + 1));
                key[1] = ss->seq[i - 1];
                key[2] = ss->seq[i];
                key[3] = '\0';
                front_p = (get_dbl_by_str(front_hashTable, (char *)&key, &front_p) == HASHNOTFOUND) ? 0.0625 * front_short_seq_num : front_p;
                front_p = front_p / front_short_seq_num;
                background_p = (get_dbl_by_str(background_hashTable, (char *)&key, &background_p) == HASHNOTFOUND) ? 0.0625 * backgroud_short_seq_num : background_p;
                background_p = background_p / backgroud_short_seq_num;
            }

            s += log(front_p) / log(background_p);

        }//end for
        //printf("%s  %f\n",key,s);
        score = s < score ? s : score;
        //printf("%f\n", s);
        tmp = tmp->next;
    }
    //获取最低分
    printf("%f\n", score);

    //获取测试集数据
    memset(path, '\0', 100);
    getcwd(path, sizeof(path));
    strcat(path, "/Testing/");

    Short_seq predicted_seq;
    Short_seq actual_seq;
    SS_INIT(&predicted_seq);
    SS_INIT(&actual_seq);

    dp = opendir(path);
    if (dp != NULL) //reading files
    {
        while(ep = readdir(dp))
        {
            if (!strcmp(ep->d_name, "."))
                continue;
            if (!strcmp(ep->d_name, ".."))
                continue;

            char file_path[100];// full file path with filename
            strcpy(file_path, path);
            strcat(file_path, ep->d_name);

            //int file_size = ftell(fp);
            fp = fopen(file_path, "r");
            //puts(file_path);
            file_to_short_seq(fp, &actual_seq, NULL);  //function in file.h
            fseek (fp, 0, SEEK_SET);
            predict_seq(fp, 12, &predicted_seq, front_hashTable, front_short_seq_num, background_hashTable, backgroud_short_seq_num);
            //puts(ep->d_name);
            fclose(fp);
        } //end while
        free(ep);
        closedir(dp);
    }
    else
    {
        printf("%s\n", "no files");
    }


    //计算Sn Sp
    double Sn;
    double Sp;

    tmp = actual_seq.queue.next;
    int actual_count = 0;
    jwHashTable *result_table;
    result_table = create_hash(10000);
    while(tmp != &(actual_seq.queue))
    {
        Short_seq *ss = (Short_seq * )tmp;
        char buff[10];
        memcpy(buff, ss->seq, 9);
        buff[9] = '\0';
        add_int_by_str(result_table, buff, 0);
        tmp = tmp->next;
        actual_count++;
    }

    tmp = predicted_seq.queue.next;
    int TP = 0;
    int false_count = 0;
    int predicted_count = 0;
    while(tmp != &(predicted_seq.queue))
    {
        Short_seq *ss = (Short_seq *)tmp;
        char buff[10];
        memcpy(buff, ss->seq, 9);
        buff[9] = '\0';
        int flag;
        if(get_int_by_str(result_table, (char *)buff, &flag) != HASHNOTFOUND)
        {
            TP++;
        }
        else
        {
            false_count++;
        }
        predicted_count++;
        tmp = tmp->next;
    }
    printf("predicted_count: %d\n", predicted_count);
    printf("Sn: %f\n", (float)TP/actual_count);
    printf("Sp: %f\n", 1 - (float)(predicted_count-TP) / predicted_count);


    /* code */
    printf("%s\n", "--------------------------------");
    return 0;
}