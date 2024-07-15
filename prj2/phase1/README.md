[system programming lecture]

-project 2 baseline

csapp.{c,h}
        CS:APP3e functions

shellex.c
        Simple shell example

단일 명령어에 대한 처리를 하는 simple shell이다.
실행시키면 프롬프트에서 명령어를 한개씩 받고,
명령어를 parsing해서 저장한 뒤에 fork함수를 통해 child process에서 해당 명령어를 execvp로 실행시키는 구조이다.
그 후, main에서 while루프를 돌기 때문에 계속 다음 명령어를 받게 되며,
만약 잘못된 명령어가 입력되면 에러 메세지를 출력한다.

