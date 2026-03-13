NAME = RPN
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS =  RPN.cpp \
	   main.cpp
HEADERS =  RPN.hpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
$(OBJS): $(HEADERS)
clean:	
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re