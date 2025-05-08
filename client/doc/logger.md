[back](index.md)

# Logger

The logger will be automatically ignored when building release without any traces in the result. \
To enable logging use -D DEBUG when compiling.

## Commands

| Command | Purpose |
|-|-|
| COMM_AWT(log) | resets logging timer and waits for COMM_CNF() as confirmation |
| COMM_MSG(colour,log) | prints a message in the given colour |
| COMM_LOG(log) | logs a message in plain white |
| COMM_SCC(log) | logs a success message in green |
| COMM_ERR(log) | logs an error message in red without timestamp |
| COMM_MSG_COND(condition,colour,log) | logs a message in a given colour on condition |
| COMM_LOG_COND(condition,log) | logs a plain white message on condition |
| COMM_SCC_COND(condition,log) | logs a success message on condition |
| COMM_ERR_COND(condition,log) | logs an error message on condition without timestamp |
| COMM_MSG_FALLBACK(colour,log) | logs a message as fallback for if statement or COMM_*_COND macro |
| COMM_LOG_FALLBACK(colour,log) | logs a plain white message as fallback for if statement or COMM_*_COND macro |
| COMM_SCC_FALLBACK(colour,log) | logs a success message as fallback for if statement or COMM_*_COND macro |
| COMM_ERR_FALLBACK(colour,log) | logs a error message as fallback for if statement or COMM_*_COND macro |

## Valid Colours

RA: LOG_(WHITE+RED+GREEN+YELLOW+BLUE+PURPLE+CYAN+GREY+CLEAR)
