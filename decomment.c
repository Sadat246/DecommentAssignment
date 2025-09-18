#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum Statetype {DEFAULT, SLASH_SEEN, INSIDE_COMMENT, 
STAR_INSIDE_COMMENT, ESCAPE_IN_STRING, ESCAPE_IN_CHARACTER_LITERAL, 
IN_STRING_LITERAL, IN_CHARACTER_LITERAL};

/* which line we are on */
int linenumber = 1; 
/* what was the line # for most recent comment*/
int startingCommentLine = 1; 
/* how many new lines we have within a comment */
int newlineCount = 0; 
/*is our last character a slash outside a comment? */
int lastCharIsSlashNotCommentMaybe= 0; 

/* handleDefaultState manages the default state. It takes
a char as a parameter. It keeps the state the same or changes
it depending on what the argument is. It also might update
lastCharIsSlashNotCommonMaybe. It outputs the character and 
returns the state. */
enum Statetype
handleDefaultState (int c)
{
	enum Statetype state;
	if (c=='/') {
	    state = SLASH_SEEN;
        lastCharIsSlashNotCommentMaybe=1;
	}
	else if (c=='"'){
	    putchar(c);
	    state=IN_STRING_LITERAL;
	}
	else if (c=='\''){
	    putchar(c);
	    state=IN_CHARACTER_LITERAL;
	}
	else{
	    putchar(c);
	    state = DEFAULT;
	}
    return state;
}

/* handleSlashSeenState manages when a slash is seen in the input.
It takes a char as a parameter. It keeps the state the same or changes
it depending on what the argument is. It also might update
lastCharIsSlashNotCommonMaybe. It outputs the character and 
returns the state. */
enum Statetype
handleSlashSeenState (int c)
{
	enum Statetype state;
	if (c=='/') {
	    putchar('/');
	    state = SLASH_SEEN;
        lastCharIsSlashNotCommentMaybe=1;
	}
	else if (c=='"'){
        putchar('/');
	    putchar(c);
	    state=IN_STRING_LITERAL;
        lastCharIsSlashNotCommentMaybe=0;
	}
	else if (c=='\''){
        putchar('/');
	    putchar(c);
	    state=IN_CHARACTER_LITERAL;
        lastCharIsSlashNotCommentMaybe=0;
	}
	else if (c=='*'){
	    startingCommentLine= linenumber;
	    state= INSIDE_COMMENT;
        lastCharIsSlashNotCommentMaybe=0;
	}
	else{
        putchar('/');
	    putchar(c);
	    state = DEFAULT;
        lastCharIsSlashNotCommentMaybe=0;
	}
    return state;
}

/* handleCommentState manages when a we are inside a comment
It takes a char as a parameter. It keeps the state the same or changes
it depending on what the argument is. It also updates
lastCharIsSlashNotCommonMaybe. It outputs the character and 
returns the state. */
enum Statetype
handleCommentState (int c)
{
	enum Statetype state;
	state = INSIDE_COMMENT;
    lastCharIsSlashNotCommentMaybe=0;
	if (c=='*') {
	    state = STAR_INSIDE_COMMENT;
	}
    else if (c == '\n') {
        newlineCount+=1;
    }
    return state;
}

/* handleStarInCommentState manages when we see a star inside a 
comment.It takes a char as a parameter. It keeps the state the same 
or changes it depending on what the argument is. It outputs the 
character and returns the state. */

enum Statetype
handleStarInCommentState (int c)
{
	enum Statetype state;
    int i;
    state = STAR_INSIDE_COMMENT;
	if (c=='/') {
	    putchar(' '); /* adding space after comment ended */
        /* for any new lines inside comment we couldn't add*/
        for (i=0;i<newlineCount;i++){ 
            putchar('\n'); 
        }
        newlineCount=0;
	    state = DEFAULT;
	}
    else if (c == '\n') {
        newlineCount+=1;
        state = INSIDE_COMMENT;
    }
	else if (c!='*'){
	    state = INSIDE_COMMENT;
	}
    return state;
}

/* handleStringLiteralState manages when we are inside a string.
It takes a char as a parameter. It keeps the state the same or changes
it depending on what the argument is. It outputs the character and 
returns the state. */
enum Statetype
handleStringLiteralState (int c)
{
	enum Statetype state;
	state = IN_STRING_LITERAL;
	if (c=='\\') {
	    state = ESCAPE_IN_STRING;
	}
	else if (c=='"'){
	    state = DEFAULT;
	}
	putchar(c);
    return state;
}

/* handleCharacterLiteralState manages when we are inside a 
character literal. It takes a char as a parameter. It keeps 
the state the same or changes it depending on what the 
argument is. It outputs the character and returns the state. */
enum Statetype
handleCharacterLiteralState (int c)
{
	enum Statetype state;
	state = IN_CHARACTER_LITERAL;
	if (c=='\\') {
	    state = ESCAPE_IN_CHARACTER_LITERAL;
	}
	else if (c=='\''){
	    state = DEFAULT;
	}
	putchar(c);
    return state;
}

/* handleCharacterEscapeState manages when we are inside a character 
literal and there is an escape. It takes a char as a parameter. 
It keeps the state the same or changes it depending on what the 
argument is. It outputs the character and returns the state. */
enum Statetype
handleCharacterEscapeState (int c)
{
	enum Statetype state;
	putchar(c);
	state = IN_CHARACTER_LITERAL;
    return state;
}

/* handleStringEscapeState manages when we are inside a string 
literal and there is an escape. It takes a char as a parameter. 
It keeps the state the same or changes it depending on what 
the argument is. It outputs the character and returns the state. */
enum Statetype
handleStringEscapeState (int c)
{
	enum Statetype state;
	putchar(c);
	state = IN_STRING_LITERAL;
    return state;
}


/* calls each helper function for dfiferent states, which are called 
when switching between the different cases for the state. We break 
after calling the function. This uses lastCharIsSlashNotCommentMaybe, 
linenumber, and newLineCount. This takes no parameters and returns 
either 0 or exit_failure.  */
int main()
{
    int c; /* this holds each char value in input */
    int i; /* this is for a for loop to add new lines. */
    enum Statetype state = DEFAULT;
    while ((c= getchar()) != EOF){
        if (c == '\n'){
	        linenumber++;
            if (state != INSIDE_COMMENT && c == '/'){
                lastCharIsSlashNotCommentMaybe=1;
            }
            else{
                lastCharIsSlashNotCommentMaybe=0;
            }
	    }
        switch (state){
            case DEFAULT:
                state = handleDefaultState(c);
                break;
            case SLASH_SEEN:
                state = handleSlashSeenState(c);
                break;
            case INSIDE_COMMENT:
                state = handleCommentState(c);
                break;
            case STAR_INSIDE_COMMENT:
                state = handleStarInCommentState(c);
                break;
            case ESCAPE_IN_STRING:
                state = handleStringEscapeState(c);
                break;
            case ESCAPE_IN_CHARACTER_LITERAL:
                state = handleCharacterEscapeState(c);
                break;
            case IN_STRING_LITERAL:
                state = handleStringLiteralState(c);
                break;
            case IN_CHARACTER_LITERAL:
                state = handleCharacterLiteralState(c);
                break;
        }
    }
    if (lastCharIsSlashNotCommentMaybe==1){
        putchar('/');
    }
    if (state == INSIDE_COMMENT || state == STAR_INSIDE_COMMENT){
        putchar(' ');
        for (i=0;i<newlineCount;i++){
            putchar('\n');
        }
        fprintf(stderr, "Error: line %d: unterminated comment\n", 
            startingCommentLine);
        return EXIT_FAILURE;

    }
    return 0;
}
