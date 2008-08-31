.PHONY: all test so clean

all: test

so:
	python setup.py build

test: so
	PYTHONPATH=./build/lib.linux-x86_64-2.5:$$PYTHONPATH python test_client.py

clean:
	rm -rf build
