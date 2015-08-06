all: blueocean

blueocean: main.c
	g++ $< -o $@ -lbluetooth

clean: blueocean
	rm blueocean
