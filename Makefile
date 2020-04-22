.PHONY: all clean run love

RAKE = rake

all:
	$(RAKE)

clean:
	$(RAKE) clobber

run:
	$(RAKE) run

love:
