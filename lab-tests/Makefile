MODULES := lab1-tests lab2-tests

.PHONY: all $(MODULES) clean test

all: $(MODULES)

$(MODULES):
	make -C $@

clean:
	for mod in $(MODULES); do \
		make -C $$mod clean; \
	done

test:
	for mod in $(MODULES); do \
		make -C $$mod test; \
	done
