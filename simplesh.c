/*第6回目課題1*/
/*画面にプロンプトを表示させ、キーボードからの入力を受け付け*/
/*新たなプロセスを生成して入力コマンドを実行するプログラム*/
/*実行方法 → ./simplesh */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //forkが使用可能
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define BUF 256

/* 最期に入る改行を取り除く関数 */
void lntrim(char *str)
{
    char *p;
    p = strchr(str, '\n');
    if (p != NULL)
    {
        *p = '\0';
    }
}

int main(void)
{

    char cmd[BUF], buf[BUF];
    char *token, *str = "date3", *str2 = "cd", *str3 = "no", *str4 = "help", *str5 = "empty", *str6="exit";
    pid_t pid;     // forkの戻り値を保存するための変数
    int pipefd[2], i=0; // パイプ用ディスクリプタ
	char *args[]={};
    const char *home_directory = getenv("HOME"); // ホームディレクトリ用の変数

    if (home_directory == NULL)
    {
        fprintf(stderr, "HOME環境変数が設定されていません。\n");
        return 1;
    }

    /*パイプ→子プロセスで処理した内容を親プロセスに伝える必要がある*/
    /*親プロセスと子プロセスのプロセス間通信をするためのもので子から親への一方通行
    子: 書き込みだけなので読み出しは閉じる, 親: 読み込みだけなので書き出しは閉じる
    int pipefd[2] → pipefd[0]: 読み出し用, pipefd[1]: 書き出し用*/

    while (1)
    {
        /*パイプの作成*/
        if (pipe(pipefd) == -1)
        {
            exit(EXIT_FAILURE);
        }

        /*子プロセス作成 forkに失敗すると-1を返す*/
        if ((pid = fork()) == -1)
        {
            printf("fork failed\n");
            close(pipefd[1]);
            close(pipefd[0]);
            continue;
        }

        memset(cmd, 0, sizeof(cmd));
        memset(buf, 0, sizeof(buf));

        /*forkの戻り値で親か子を判断(pid=0: 子, pid>0: 親)*/
        /*子プロセスの処理*/
        if (pid == 0)
        {
            close(pipefd[0]); // 読み込みディスクリプタを閉じる
            //printf("子プロセスです. ID=%d\n", getpid());
            printf("PROMPT>");

 			//文字列入力
            if(fgets(cmd, BUF, stdin)==NULL){
                printf("エラーです\n");
            }else if(strlen(cmd)==1){	//エンターだけ入力された時
				
				write(pipefd[1], "empty", 5); 
                close(pipefd[1]);
				exit(0);
            }
            
            lntrim(cmd); // 改行文字を消す
            
            // 文字列にスペースがあればスペースまでの1つ目の文字列の最初のアドレスをtokenは持っている
            token = strtok(cmd, " ");
           
			
		    // printf("最初のtokenの文字列: %s 数字%d\n",token,  strlen(token));
			if(strcmp(token, str4)==0){
				printf("helpオプション\n[コマンド名] [オプション]\n[date3]: 独自コマンド");
			}
			if(strcmp(token, str6)==0){
				write(pipefd[1], "exit", 4);
				close(pipefd[1]);
				exit(0);
			}
            /*cdコマンド*/
            if (strcmp(token, str2) == 0)
            {   
                // tokenのアドレスを2つ目の文字列の先頭文字に移動
                token = strtok(NULL, " ");
                // パイプに書き込み
                write(pipefd[1], token, strlen(token) + 1);
                close(pipefd[1]);
            }
            else if(strcmp(token, str)==0){
					printf("  ∧_∧  \n ∧_∧･ω･`)\n( ･ω･`)･ω･`)\nく|　⊂)ω･`)\n　 (⌒　ヽ･`)\n ∪￣＼⊃\n");
			}else{
                // 2つ目も文字列がない場合、NULLやEOF、改行で判定したがうまくいかないので、"no"で識別
                write(pipefd[1], "no", 2);
                close(pipefd[1]);

				while(token!=NULL){
					args[i]=token;
					token=strtok(NULL, " ");
					//printf("配列への挿入: %s\n", args[i]);
					i++;
				}		
			
                if (execvp(args[0], args) == -1)
                {
                    printf("そのようなコマンドはありません");
					printf("次のコマンドでヘルプを見れます→help\n");
                }
            }

            // 子プロセス終了
            exit(0);
        }

        /*親プロセスの処理*/
        else if (pid > 0)
        {
            close(pipefd[1]); // 書き込みディスクリプタを閉じる
            //printf("親プロセスです. ID=%d, 子のID=%d\n", getpid(), pid);

            wait(NULL); // 子プロセスの終了を待つ

            read(pipefd[0], buf, sizeof(buf)); // パイプの文字を読み取る
        
            // cdコマンドかそうでないかの判別
            if (strcmp(str3, buf) == 0)
            {
             	//分類のため
            }else if(strcmp(str6, buf)==0){
				exit(1);
			}
            else if(strcmp(str5, buf)==0){
				//分類のため
			}else{
                //cdコマンドだけの時
                if (strlen(buf) == 0)
                {
                    // homedirectoryに移動
                    chdir(home_directory);
                }
                else
                {
                    chdir(buf);
                }
            }
            close(pipefd[0]);
        }
    }
    return 0;
}
