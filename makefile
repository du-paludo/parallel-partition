# Eduardo Stefanel Paludo - GRR20210581
# Natael Pontarolo Gomes - GRR20211786

flags = -Wall
name = multi-partition

all: $(name)

$(name): main.o chrono.o
	gcc -o $(name) main.o chrono.o $(flags)

main.o: main.c
	gcc -c main.c $(flags)

chrono.o: chrono.c
	gcc -c chrono.c $(flags)

clean:
	rm -f *~ *.o

purge: clean
	rm -f bsearch-A bsearch-B
