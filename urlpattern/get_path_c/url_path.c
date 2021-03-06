 #include "stdio.h"  
 #include "string.h"  
 #include "stdlib.h"  
 #include "list.c"  
 #include "hash.c"
 #include "ic_tools.c"
 #include "time.h"
//
#define MIN_COM_STING 4
#define MAX_COM_STING 20
///
   struct com_str_res{
       int  max;
       char  longest_str[1024];
   };
   //
   struct url_res{
       struct url_path_list * list;
       struct url_mhash * mh;
   };
   //
   struct url_list_res{
       struct url_path_list * digital_list;
       struct url_path_list * undigital_list;
   };
   //
   struct com_str_res *common_string(char *str1, char *str2)  
   {  
       struct com_str_res * com_str;
       com_str = (struct com_str_res *)malloc(sizeof(struct com_str_res));
       memset(com_str, 0 , sizeof(struct com_str_res));
       int i,j,k,len1,len2,max,x,y;  
       len1 = strlen(str1);  
       len2 = strlen(str2);  
       int **c = (int **)malloc(sizeof(int *) *(len1 + 1));  
       for(i = 0; i < len1+1; i++) { 
           c[i] = (int *)malloc(sizeof(int *) *(len2 + 1));  
       }
       for(i = 0; i < len1+1; i++) {
           c[i][0]=0;        //第0列都初始化为0  
       }
       for(j = 0; j < len2+1; j++) { 
           c[0][j]=0;        //第0行都初始化为0   
       }
       max = -1;  
       for(i = 1 ; i < len1+1 ; i++)  
       {  
           for(j = 1; j < len2+1; j++)  
           {  
               if(str1[i-1]==str2[j-1])     //只需要跟左上方的c[i-1][j-1]比较就可以了  
               c[i][j]=c[i-1][j-1]+1;  
               else                         //不连续的时候还要跟左边的c[i][j-1]、上边的c[i-1][j]值比较，这里不需要  
               c[i][j]=0;  
               if(c[i][j]>max)  
               {
                   max=c[i][j];  
                   x=i;  
                   y=j;  
               }  
           }  
       }  
       char s[1024];  
       k=max;  
       i=x-1,j=y-1;  
       s[k--]='\0';  
       while(i>=0 && j>=0)  
       {  
           if(str1[i]==str2[j])  
           {  
               s[k--]=str1[i];  
               i--; 
               j--; 
           }  
           else{       //只要有一个不相等，就说明相等的公共字符断了，不连续了  
                break;
               }  
       }
       com_str->max = max;
       //com_str->longest_str = s;
       memmove(com_str->longest_str, s, 1024);
       //printf("最长公共子串为：");  
       //puts(s);  
       for(i = 0; i < len1+1; i++) {
           free(c[i]); 
       }
       free(c); 
       return com_str;  
   }
   
  int judge_digital(char * url_path) {
      char  str[4096];
      strcpy(str,url_path);
      int flag = 0;
      int i = 0;
      for(i = 0; i < strlen(str); i++ ) {
          if((str[i] >= 'a' && str[i] <= 'z')){
              flag = 1;
              break;;
          }else{
              continue;
          }
      }
      return flag;
  }

  void url_list_res_free(struct url_list_res * list_res) {
      if(list_res == NULL) {
          return;
      }
      struct url_path_list_node * node;
      struct url_path_list_node * tmp_node;
      if(list_res->digital_list) {
          url_path_list_free_all(list_res->digital_list);
      }
      if(list_res->undigital_list) {
          url_path_list_free_all(list_res->undigital_list);
      }
      free(list_res);
  }

  struct url_list_res * url_path_list_insert(char * file) {
      char  url_tmp[4096];
      memset(url_tmp, 0, sizeof(url_tmp));
      //
      FILE * fp;
      //char line[1024];
      // 
      struct url_list_res * list_res = (struct url_list_res *)malloc(sizeof(struct url_list_res));
      memset(list_res, 0, sizeof(struct url_list_res));
      struct url_path_list * undigital_list;
      struct url_path_list * digital_list;
      //
      fp = fopen(file,"r");
      //
      undigital_list = url_path_list_init(); 
      digital_list = url_path_list_init();
      //
      while(fgets(url_tmp,1024,fp) != NULL) {
          char * loc_abs_path = NULL;
          char * loc_host_path = NULL;
          char * tail_path = NULL;
          char * del_host_path = NULL;
          int len;
          char * url = NULL;
          char * tmp = NULL;
          char * url_p = NULL;
          char  buf[1024];
          memset(buf, 0, sizeof(buf));
          //
          url = url_tmp;
          loc_host_path = strchr(url_tmp, ':');
          if(loc_host_path == NULL || url_tmp == NULL) {
              continue;
          }
          len = loc_host_path - url_tmp;//域名长度
          strncpy(buf, url_tmp + len + 3, strlen(url_tmp));
          //printf("%s\n",buf);      //除去http://之后的内容
          //loc_abs_path = (char *)malloc(strlen(buf));
          del_host_path = strchr(buf,'/');
          if(del_host_path == NULL) {
              continue;
          }
          //处理以.html、.json等结尾的url
          tail_path = strchr(del_host_path, '.');
          //处理以汉字编码%E%.....结束的url
          if(tail_path == NULL) {
              tail_path = strchr(del_host_path, '%');
          }
          if(tail_path != NULL) {
              loc_abs_path = (char *)malloc((tail_path - del_host_path + 1));
              memset(loc_abs_path, 0, tail_path - del_host_path + 1);
              strncpy(loc_abs_path, del_host_path,tail_path - del_host_path);
          }else{
              loc_abs_path = (char *)malloc(strlen(del_host_path) + 1);
              memset(loc_abs_path, 0, strlen(del_host_path) + 1);
              strcpy(loc_abs_path, del_host_path);
          }
          if(loc_abs_path == NULL) {
              continue;
          }
          tmp = (char *)malloc(strlen(loc_abs_path) + 1);
          memset(tmp, 0, strlen(loc_abs_path) + 1);
          strcpy(tmp,loc_abs_path);
          url_p = (char *)malloc(strlen(url) + 1);
          strcpy(url_p,url);
          //printf("return_before:%s",loc_abs_path);     //除去host后的绝对路径
          if(judge_digital(tmp) == 1) {
              url_path_list_add(undigital_list, tmp, strlen(tmp) + 1, url_p, strlen(url_p) + 1);
          }else{
              url_path_list_add(digital_list, url_p, strlen(url_p) + 1, 0, 1);
              free(tmp);
          }
           free(loc_abs_path);
          //printf("tmp:%s\nhead:%s\n",tmp,list->head->key);
       }
        fclose(fp);
        list_res->undigital_list = undigital_list;
        list_res->digital_list = digital_list;
        //free(tmp);
        //free(loc_abs_path);
        ////放在哪里free while循环里？or外？
      //
      /*struct url_path_list_node * node;
      node = list_res->undigital_list->head;
      while(node != NULL) {
          printf("key:%s\tvalue:%s",node->key,node->value);
          node = node->next;
      }*/
      return list_res;
      //url_path_list_free_all(undigital_list);
      //url_path_list_free_all(digital_list);
  }


  struct url_res * get_res(struct url_list_res * list){
      clock_t start;
      clock_t end;
      struct url_path_list_node * node1;
      struct url_path_list_node * node2;
      char * str1 = 0;
      char * str2 = 0;
      //
      struct com_str_res * com_string;
      char * longest_str_key = 0;
      char * longest_str = 0;
      char * longest_str_tmp = 0;
      char * start_str = 0;
      char * url_1 = 0;
      char * url_2 = 0;
      //
      struct url_path_list * url_list;
      url_list = url_path_list_init();
      struct url_mhash * mh;
      struct url_path_list_node * node;
      //int status;
      //
      struct url_path_list * str_list;
      str_list = url_path_list_init();
      //
      struct url_res * result = (struct url_res *)malloc(sizeof(struct url_res));
      mh = url_mhash_init(100007);
      //处理数字path
      char * url_digital = 0;
      char * digital_key = "digital_path";
      char * digital_key_for_mh = (char *)malloc(strlen(digital_key)+1);
      strcpy(digital_key_for_mh, digital_key);
      url_path_list_add(str_list, digital_key, strlen(digital_key) + 1, 0, 1);
      start = clock();
      node = list->digital_list->head;
      while(node != NULL) {
          url_mhash_put(mh, digital_key_for_mh, url_list);
          url_digital = (char *)malloc(strlen(node->key) + 1);
          strcpy(url_digital, node->key);
          url_path_list_add(url_list, url_digital, strlen(url_digital) + 1, 0, 1);
          //printf("%s\t%s",digital_key,node->key);
          node = node->next;
      }
      end = clock();
      //printf("digital_list_time: %f\n",(double)(end - start)/CLOCKS_PER_SEC);
      //////
      //处理非数字path
      node1 = list->undigital_list->head;
      while(node1 != NULL) {
         // status = 0;
          str1 = (char *)malloc(strlen(node1->key) + 1);
          strcpy(str1,node1->key);
          //printf("str1:%s\tnode1->key:%s",str1,node1->key);
          node2 = node1->next;
          start = clock();
          while(node2 != NULL) {
              str2 = (char *)malloc(strlen(node2->key) + 1);
              strcpy(str2,node2->key);
              //printf("str2:%s\tnode2->key:%s",str2,node2->key);
              com_string = common_string(str1, str2);
              //剔除url_path过短以及是纯数字的情况
              if(com_string->longest_str == NULL || com_string->max < MIN_COM_STING || judge_digital(com_string->longest_str) == 0) {
                  node2 = node2->next;
                  free(str2);
                  free(com_string);
                  continue;
              }
              ////
              start_str = (char *)malloc(strlen(com_string->longest_str) + 1);
              strcpy(start_str, com_string->longest_str);
              //处理数字+url_path的情况
              char * head_str;
              longest_str_tmp = strchr(start_str,'/');
              head_str = substr(start_str, 0, longest_str_tmp - start_str);
              if(longest_str_tmp != NULL && judge_digital(head_str) == 0) {
                  longest_str = longest_str_tmp;
              }else{
                  longest_str = start_str;
              }
              //处理url_path+数字的情况
              if((longest_str_tmp = strrchr(longest_str, '/')) != NULL && judge_digital(longest_str_tmp) == 0) {
                  longest_str_key = substr(longest_str, 0, longest_str_tmp - longest_str);
              }else{
                  longest_str_key = (char *)malloc(strlen(longest_str) + 1);
                  strcpy(longest_str_key, longest_str);
              }
              //再次删除过短、过长以及非路径名称字符串
              if(strlen(longest_str_key) < MIN_COM_STING || longest_str_key[0] != '/' || strlen(longest_str_key) > MAX_COM_STING) {
                  node2 = node2->next;
                  free(str2);
                  free(start_str);
                  free(com_string);
                  free(head_str);
                  free(longest_str_key);
                  continue;
              }
              ///
              url_1 = (char *)malloc(strlen(node1->value) + 1);
              strcpy(url_1, node1->value);
              url_2 = (char *)malloc(strlen(node2->value) + 1);
              strcpy(url_2, node2->value);
              //
              if((url_list = (struct url_path_list*)url_mhash_get(mh, longest_str_key)) == NULL) {
                  url_list = url_path_list_init();
                  //
                  url_mhash_put(mh, longest_str_key, url_list);
                  url_path_list_add(url_list, url_1, strlen(url_1) + 1, 0, 1);
                  url_path_list_add(url_list, url_2, strlen(url_2) + 1, 0, 1);
                  url_path_list_add(str_list, longest_str_key, strlen(longest_str_key) + 1, 0, 1);
                  node1->status = 1;
                  node2->status = 1;
              }else{
                  url_list = url_mhash_get(mh, longest_str_key);
                  if((url_path_list_uniq(url_list, url_1)) == 0) {//去重
                      url_path_list_add(url_list, url_1, strlen(url_1) + 1, 0, 1);
                      node1->status = 1;
                  }else{ 
                      free(url_1);
                  }
                  if((url_path_list_uniq(url_list, url_2)) == 0) {
                      url_path_list_add(url_list, url_2, strlen(url_2) + 1, 0, 1);
                      node2->status = 1;
                  }else{
                      free(url_2);
                  }
                  free(longest_str_key);
              }
              node2 = node2->next;
              free(str2);
              free(head_str);
              free(start_str);
              free(com_string);
          }
          end = clock();
          //printf("node2_time: %f\n",(double)(end - start)/CLOCKS_PER_SEC);
          /*if(node1->status != 1) {
              longest_str_key = (char *)malloc(strlen(node1->key) + 1);
              strcpy(longest_str_key, node1->key);
              //printf("%d\n%s\n%s\n",node1->status,longest_str_key,node1->value);
              if((url_list = (struct url_path_list*)url_mhash_get(mh, longest_str_key)) == NULL) {
                  url_list = url_path_list_init();
                  url_mhash_put(mh, longest_str_key, url_list);
                  url_1 = (char *)malloc(strlen(node1->value) + 1);
                  strcpy(url_1, node1->value);
                  url_path_list_add(url_list, url_1, strlen(url_1) + 1, 0, 1);
                  url_path_list_add(str_list, longest_str_key, strlen(longest_str_key) + 1, 0, 1);
              }else{
                  free(longest_str_key);
              }
          }*/
          node1 = node1->next;
          free(str1);
      }
      result->list = str_list;
      result->mh = mh;
      //url_list_res_free(list);
      return result;
  }
  //
  void url_res_free(struct url_res * res) {
      struct url_mhash_node * node;
      size_t i = 0;
      //
      if(res == NULL) {
          return;
      }
      //
      if(res->list) {
          url_path_list_free(res->list);
      }
      if(res->mh) {
          for(i=0; i<res->mh->basic_size; i++) {
              node = res->mh->head + i;
              if(node->value != NULL) {
                  url_path_list_free_all((struct url_path_List *)(node->value));
                  node->value = 0;
              }
              node = node->next;
              while(node != NULL) {
                  if(node->value != NULL) {
                      url_path_list_free_all((struct url_path_List *)(node->value));
                      node->value = 0;
                  }
                  node = node->next;
              }
          }
          url_mhash_free_all(res->mh);
      }
      free(res);
  }


   int main(void)  {
       /*char * str = "/4566/1234/34";
       char * string = "/345/sjkk/355";
       int flag_1 = judge_digital(str);
       int flag_2 = judge_digital(string);
       printf("%d\n%d\n",flag_1,flag_2);*/
       //
       char * file = "./url_data_1000";
       struct url_list_res * list_res;
       list_res = url_path_list_insert(file);
       //
       /*struct url_path_list_node * node1;
       struct url_path_list_node * node2;
       node1 = list_res->digital_list->head;
       while(node1 != NULL) {
          printf("key:%s\tvalue:%s",node1->key, node1->value);
          node1 = node1->next;
      }
       node2 = list_res->undigital_list->head;
       while(node2 != NULL) {
          printf("key:%s\tvalue:%s",node2->key, node2->value);
          node2 = node2->next;
      }*/
       //
       struct url_res * res;
       struct url_mhash * mh;
       struct url_path_list_node * node_str;
       struct url_path_list_node * node;
       struct url_path_list * str_list;
       struct url_path_list * url_list;
       res = get_res(list_res);
       str_list = res->list;
       mh = res->mh;
       node = str_list->head;
       while(node!=NULL){
           url_list = url_mhash_get(mh, node->key);
           printf("url_key:%s\n", node->key);
           node_str = url_list->head;
           int count = 0;
           while(node_str != NULL) {
               count ++;
               printf("url:%s", node_str->key);
               node_str = node_str->next;
           }
           printf("count: %s\t%d\n", node->key, count);
           node = node->next;
       }
       url_list_res_free(list_res);
       url_res_free(res);
       //
       return 0; 
   }  
