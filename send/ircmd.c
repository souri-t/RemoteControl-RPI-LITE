#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

/************************************************/
/*  ircmd.c                                     */
/************************************************/

/*  コマンド名    */
#define SEND "SEND"
#define FILES "FILES"
#define COMMANDS "COMMANDS"

int main(int argc, char** argv)
{
    /*  コマンドライン引数から送信先を取得する    */
    char* destination = argv[1];

    /*  送信コードを取得する    */
    char* commandName = argv[2];

    /*  送信文字列 */
    char sendMessage[124];
    memset(sendMessage, 0x00, sizeof(sendMessage));

    /*  デバッグ出力    */
    printf("commandName:%s\n", commandName);
    printf("destination:%s\n", destination);

    /**********************/
    /*  コマンド判定        */
    /**********************/
    {
        /** 送信コマンド    */
        if(strcmp(commandName, SEND) == 0){
            if(argc != 5)return -1;
            sprintf(sendMessage, "[sendir]sendir %s %s", argv[3], argv[4]);
        }
        /* ファイル名取得コマンド    */
        else if(strcmp(commandName, FILES) == 0){
            if(argc != 3)return -1;
            strcpy(sendMessage, "[loadfiles]");
        }
        /* コマンド名取得コマンド    */
        else if(strcmp(commandName, COMMANDS) == 0){
            if(argc != 4)return -1;
            sprintf(sendMessage, "[loadsendcommands]%s", argv[3]);
        }else{
            return -1;
        }
    }

    /**********************/
    /*  ソケット通信        */
    /**********************/
    {
        int sock;
        struct sockaddr_in addr;

        // IPv4 TCP のソケットを作成する
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            return -1;
        }
    
        // 送信先アドレスとポート番号を設定する
        addr.sin_family = AF_INET;
        addr.sin_port = htons(29920);
        addr.sin_addr.s_addr = inet_addr(destination);
    
        // サーバ接続（TCPの場合は、接続を確立する必要がある）
        connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        
        // パケットをTCPで送信
        if(send(sock, sendMessage, strlen(sendMessage), 0) < 0) {
            perror("send");
            return -1;
        }

        /* サーバからデータを受信 */
        char buf[64];
        memset(buf, 0, sizeof(buf));
        int n = read(sock, buf, sizeof(buf));

        printf("return:%s\n", buf);

        close(sock);
    }
    return 0;
}