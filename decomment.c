#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum Statetype {DEFAULT, SLASH_SEEN, INSIDE_COMMENT, STAR_INSIDE_COMMENT, ESCAPE_IN_STRING, ESCAPE_IN_CHARACTER_LITERAL, IN_STRING_LITERAL, IN_CHARACTER_LITERAL};

int linenumber = 1;
int startingCommentLine = 1;
int newlineCount = 0;
int lastCharIsSlashNotCommentMaybe= 0; /*false*/
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
        lastCharIsSlashNotCommentMaybe=0;
	}
	else if (c=='\''){
	    putchar(c);
	    state=IN_CHARACTER_LITERAL;
        lastCharIsSlashNotCommentMaybe=0;
	}
	else{
	    putchar(c);
	    state = DEFAULT;
        lastCharIsSlashNotCommentMaybe=0;
	}
    return state;
}
    
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

enum Statetype
handleStarInCommentState (int c)
{
	enum Statetype state;
    int i;
    lastCharIsSlashNotCommentMaybe=0;
    state = STAR_INSIDE_COMMENT;
	if (c=='/') {
	    putchar(' ');
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

enum Statetype
handleStringLiteralState (int c)
{
	enum Statetype state;
	state = IN_STRING_LITERAL;
    lastCharIsSlashNotCommentMaybe=0;
	if (c=='\\') {
	    state = ESCAPE_IN_STRING;
	}
	else if (c=='"'){
	    state = DEFAULT;
	}
	putchar(c);
    return state;
}

enum Statetype
handleCharacterLiteralState (int c)
{
	enum Statetype state;
	state = IN_CHARACTER_LITERAL;
    lastCharIsSlashNotCommentMaybe=0;
	if (c=='\\') {
	    state = ESCAPE_IN_CHARACTER_LITERAL;
	}
	else if (c=='\''){
	    state = DEFAULT;
	}
	putchar(c);
    return state;
}

enum Statetype
handleCharacterEscapeState (int c)
{
	enum Statetype state;
    lastCharIsSlashNotCommentMaybe=0;
	putchar(c);
	state = IN_CHARACTER_LITERAL;
    return state;
}

enum Statetype
handleStringEscapeState (int c)
{
	enum Statetype state;
    lastCharIsSlashNotCommentMaybe=0;
	putchar(c);
	state = IN_STRING_LITERAL;
    return state;
}


int main()
{
    int c;
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
        fprintf(stderr, "Error: line %d: unterminated comment\n", startingCommentLine);
        return EXIT_FAILURE;

    }
    return 0;
}
