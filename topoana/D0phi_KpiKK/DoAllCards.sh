mkdir -p pdf
mkdir -p tex
for i in $(ls | grep .card); do
	topoana.exe "${i}"
	mv "${i/card/pdf}" pdf
	mv "${i/card/tex}" tex
	rm "${i/card/root}"
	rm "${i/card/txt}"
done