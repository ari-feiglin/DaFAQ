#include "dafaq.h"

/**
 * @brief: Prints a line of spaces with indents of some color
 * @param[IN] indent_len: The length of the index
 * @param[IN] row_length: The length of the line (including indents)
 * @param[IN] centered: True if indents on both sides, else false
 * @param[IN] r: Red color in indent
 * @param[IN] g: Green color in indent
 * @param[IN] b: Blue color in indent
 */
void print_indent_line(IN int indent_len, IN int row_length, IN bool centered, IN int r, IN int g, IN int b){
    int i = 0;

    print_color("`", BG,r,g,b);
    for(i=0; i<indent_len; i++){
        printf(" ");
    }
    print_color("`", BG,0,0,0);
    for(i=0; i<row_length - indent_len*(centered+1); i++){
        printf(" ");
    }
    print_color("`", BG,r,g,b);
    for(i=0; i<indent_len*centered; i++){
        printf(" ");
    }

    print_color("~", RESET);
    printf("\033[%dD", row_length-indent_len);
}

/**
 * @brief: Prints the the title of the system (DaFAQ)
 * @param[IN] row_length: Length of each line
 * 
 * @notes: Like with sprint_menu, I have unnecessary prints. These will be fixed... soon?
 */
void print_title(IN int row_length){
    char * title[] = {  
                        "#########                                                ",
                        "##      ##              ########    #######   #########  ",
                        "##      ##              ##         ##    ##  ##       ## ",
                        "##      ##              ##         ##    ##  ##       ## ",
                        "##      ##    ######    #####      ########  ##       ## ",
                        "##      ##   ##    ##   ##         ##    ##  ##       ## ",
                        "##      ##   ##    ##   ##         ##    ##  ##   ### ## ",
                        "#########     ##### ##  ##         ##    ##    ########  ",
                        "                                                      ###"
                    };
    int i = 0;
    int j = 0;
    int line_len = 0;
    int title_len = 0;
    int green = 200;
    char * subtitle = " The Near Useless Database Engine ";
    int subtitle_len = 0;

    subtitle_len = strnlen(subtitle, STRING_LEN);
    title_len = sizeof(title) / sizeof(title[0]);
    line_len = strnlen(title[i], STRING_LEN);

    printf("\n");

    print_indent_line(0, row_length, true, 0,green,255);
    print_color("~\n", RESET);

    for(i=0; i<title_len; i++, green-=15){
        line_len = strnlen(title[i], STRING_LEN);
        print_color("`", BG,0,0,0);
        simple_center_text(line_len, row_length);

        for(j=0; j<line_len; j++){
            switch(title[i][j]){
                case '#':
                    print_color("` ", BG,0,green,255);
                    break;
                case ' ':
                    print_color("` ", BG,0,0,0);
                    break;
            }
        }
        print_color("~\n", RESET);
    }

    print_indent_line(0, row_length, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0, 0,200,255);
    
    print_indent_line(0, row_length, true, 0,200,255);
    print_color("`", BG,0,0,0);
    simple_center_text(subtitle_len, row_length-4);
    print_color("``~~", BG,0,200,255, FG,0,0,255, BOLD, ITALIC);
    printf("%s", subtitle);
    print_color("~\n", RESET);

    print_indent_line(0, row_length, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(0, row_length, true, 0,200,255);
    print_color("~\n", RESET);
}

/**
 * @brief: Prints the menu GUI
 * @param[IN] cursor_pos: The position of the cursor
 * 
 * @notes: This function has many unnecessary prints. It is the "secret" (non-public) function used by print_menu
 */
int sprint_menu(IN int cursor_pos){
    int row_len = 75;

    system("clear");
    
    print_title(75);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(0 != cursor_pos){
        print_color("`  ` `  ~Create New Database~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Create New Database~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }
    

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(1 != cursor_pos){
        print_color("`  ` `  ~Create New Table~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Create New Table~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(2 != cursor_pos){
        print_color("`  ` `  ~Edit Table~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Edit Table~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(3 != cursor_pos){
        print_color("`  ` `  ~Create New Record~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Create New Record~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(4 != cursor_pos){
        print_color("`  ` `  ~Edit Record~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Edit Record~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(5 != cursor_pos){
        print_color("`  ` `  ~Poop Table~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Poop Table~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(6 != cursor_pos){
        print_color("`  ` `  ~Diarrhea Database~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Diarrhea Database~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(7 != cursor_pos){
        print_color("`  ` `  ~Execute Query~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Execute Query~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }
    
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(0, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(0, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(8 != cursor_pos){
        print_color("`  ` `  ~About~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~About~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(9 != cursor_pos){
        print_color("`  ` `  ~Credits~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Credits~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    if(10 != cursor_pos){
        print_color("`  ` `  ~Usage~\n", BG,0,0,0, BG,0,200,255, BG,0,0,0, BOLD, RESET);
    }
    else{
        print_color("`  ` `  ~Usage~\n", BG,0,0,0, BG,255,255,255, BG,0,0,0, BOLD, RESET);
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(0, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    
}

/**
 * @brief: Prints the about section
 * @param[IN] row_length: Length of each line
 */
void print_about(IN int row_len){
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~The DaFAQ Database System was created by Ari Feiglin for no good reason at all~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~The first line of code was written on October 2, 2020~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~The reason I created the program is because my class was using Microsoft Access, and as a Linux user, I felt like my alternatives~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~(LibreOffice Base) were too modern. Like, who needs fancy relationships and forms, a nice GUI, and a non text-based interface?~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~Also, I had created a simple banking system for/because of a friend, and this also helped give me the idea to create DaFAQ.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~So, work began on October 2nd and Version 0.0.0 was released on October 5th. It was a pretty simple program, and you couldn't do much with it~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~I'm not saying that this current version is any good, either. It is still simple, and nearly useless.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~Hopefully in the future this will become ~almost~`~ useless. Hopefully one day this will actually function well. Hopefully.~\n", BG,0,0,0, BOLD, ITALIC, RESET, BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
}

/**
 * @brief: Prints the credits section
 * @param[IN] row_length: Length of each line
 */
void print_credits(IN int row_len){
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~I am proud to say that almost all of the code in this program was written by me, and me only~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~The only piece of code taken from someone/thing other than me was the code for the iterative quicksort, which was taken from GeeksForGeeks~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~Everything else I wrote by myself.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~But, that's not to say that nothing else inspired me. So here are a few people that inspired me to create DaFAQ~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~* First off, we have my school, which is forcing me to learn about making databases with MS Access. Without them I would never have made~\n", BG,0,0,0, BOLD, RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~  DaFAQ probably.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~* Next, there's Illay Kaye (my friend) for whom I made my simple bank managment system. You can find his GitHub here: ~https://github.com/illayK~\n", BG,0,0,0, BOLD, UNDERLINE, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~* Of course there's my database class teacher.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~* This goes without saying, but all of those online proramming forums/wikis/etc.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~* And everyone else who has helped me with this project.~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
}

/**
 * @brief: Handles clicks on the menu GUI
 * @param[IN] cursor_pos: position of the cursor (to determine which button was clicked)
 */
error_code_t click_handler(int cursor_pos){
    int error_check = ERROR_CODE_SUCCESS;
    char * input = NULL;
    char * sort_file = NULL;
    char quit = 0;
    bool continue_loop = true;
    int row_len = 75;
    int difference = 0;

    system("clear");
    if(cursor_pos > 7){
        row_len = 150;
    }
    print_title(row_len);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    change_echo(true);

    switch(cursor_pos){
        case 0:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~New Database Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                print_indent_line(1, row_len, true, 0,200,255);
                print_color("~\n", RESET);
                print_indent_line(2, row_len, true, 0,200,255);
                print_color("~\n", RESET);
                break;
            }

            error_check = create_database(input);
            /*if(ERROR_CODE_SUCCESS != error_check){
                goto cleanup;
            }*/
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            break;

        case 1:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Table Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                break;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(0, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            error_check = create_table_interface(input);
            if(-1 != error_check){
                error_check = ERROR_CODE_SUCCESS;
            }

            break;

        case 2:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Table Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                break;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(0, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            error_check = switch_field_interface(input);
            /*if(ERROR_CODE_SUCCESS != error_check){
                goto cleanup;
            }*/

            break;

        case 3:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Table Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                break;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(0, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            error_check = switch_record_interface(input, -1);
            if(-1 != error_check){
                error_check = ERROR_CODE_SUCCESS;
            }

            break;

        case 4:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Table Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                break;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(0, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            error_check = switch_record_interface(input, -2);
            if(-1 != error_check){
                error_check = ERROR_CODE_SUCCESS;
            }

            break;

        case 5:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Table Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                break;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(0, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            error_check = poop(input, NULL, false);
            if(-1 != error_check){
                error_check = ERROR_CODE_SUCCESS;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            break;

        case 6:
            error_check = diarrhea(".", NULL);
            if(-1 != error_check){
                error_check = ERROR_CODE_SUCCESS;
            }

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            break;

        case 7:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Table Name:~` ", BG,0,0,0, FG,0,200,255, BOLD, RESET, BG,0,0,0);
            error_check = get_raw_input(NULL, &input);
            if(-1 == error_check){
                goto cleanup;
            }

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                error_check = ERROR_CODE_SUCCESS;
                break;
            }

            if(NULL != sort_file){
                free(sort_file);
            }
            sort_file = malloc(error_check + extension_len + 1);
            if(NULL == sort_file){
                print_indent_line(1, row_len, true, 0,200,255);
                print_color("`  `~", BG,0,0,0, FG,255,0,0, BOLD);
                perror("CLICK HANDLER: Malloc error");
                print_color("~", RESET);
                error_check = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
                goto cleanup;
            }

            sprintf(sort_file, "%s.srt", input);

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(0, row_len, true, 0,200,255);
            print_color("~\n", RESET);

            error_check = query_interface(input, sort_file);
            /*if(ERROR_CODE_SUCCESS != error_check){
                goto cleanup;
            }*/

            break;

        case 8:
            print_about(150);
            break;

        case 9:
            print_credits(150);
            break;

        case 10:
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  ~~Unavailable at the moment. Read the README, details/Usage, and docstrings~\n", BG,0,0,0, RED, BOLD, RESET);
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("`\n~", RESET, BG,0,0,0);
            break;

        default:
            print_color("`  ~~ERROR! Non-existent button!~\n", BG,0,0,0, RED, BOLD, RESET);
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~\n", RESET);
            print_indent_line(2, row_len, true, 0,200,255);
            print_color("`\n~", RESET, BG,0,0,0);
            break;
    }

    print_indent_line(0, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    
    print_color("\n~--------------------------------------------------~\n\n", B_BLUE, RESET);
    while(continue_loop){
        print_color("~~Input 'q' (the enter) to quit:~~ ", BLUE, BOLD, RESET);
        quit = getchar();
        if('q' == quit){
            continue_loop = false;
        }
        while('\n' != quit){
            //printf("Not flushed\n");
            quit = getchar();
        }
    }

    //error_check = ERROR_CODE_SUCCESS;
cleanup:
    if(NULL != input)
        free(input);
    if(NULL != sort_file)
        free(sort_file);

    return error_check;
}

/**
 * @brief: Prints the menu GUI and passes along clicks
 */
int print_menu(){
    int row_len = 75;
    char input = 0;
    int cursor_pos = 0;
    int error_check = 0;
    bool valid_input = false;

    change_echo(false);

    while(true){
        valid_input = false;
        if(cursor_pos > 10){
            cursor_pos = 0;
        }
        else if(cursor_pos < 0){
            cursor_pos = 10;
        }
        
        sprint_menu(cursor_pos);

        while(!valid_input){
            input = getchar();
            if('w' == input){
                cursor_pos--;
                valid_input = true;
            }
            else if('s' == input){
                cursor_pos++;
                valid_input = true;
            }
            else if('\n' == input){
                valid_input = true;
                error_check = click_handler(cursor_pos);
                if(ERROR_CODE_SUCCESS != error_check){
                    print_color("~~AN ERROR HAS OCCURRED.\nPlease view previous error messages and determine if you would like to quit the program. (q to quit, then enter)~\n", BOLD, RED, RESET);
                    input = getchar();
                    if('q' == input){
                        goto cleanup;
                    }
                }
                change_echo(false);
            }
        }
    }

cleanup:
    change_echo(true);

    return error_check;
}
