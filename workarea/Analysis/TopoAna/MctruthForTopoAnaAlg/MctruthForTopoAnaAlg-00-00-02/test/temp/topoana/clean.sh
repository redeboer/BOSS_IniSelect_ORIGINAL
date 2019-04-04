#!/bin/bash
files="*topoana.pdf *topoana.root *topoana.tex *topoana.txt"
for file in $files
do
if [ -e $file ]
then
rm $file
echo "The file $file has been removed !"
else
echo "The file $file does not exist !"
fi
done
exit 0
