#! /bin/bash
# $1 - The l7 protocol name
# $2 - The first $2 bytes for each direction tcp flow


cd ~/workspace_for_github/samples
if test -d $1_samples
then
	echo "The directory " $1_samples "exists."
else
	mkdir $1_samples
fi
	
cd $1_samples

if test -d $2
then
	echo "The directory " $1_samples"/"$2 "exists."
else
	mkdir $2
fi

cd $2

if test -d cs
then
	echo "The dirctories cs exists."
else
	mkdir cs
fi

if test -d sc
then
	echo "The dirctories sc exists."
else
	mkdir sc 
fi

if test -d bd 
then
	echo "The dirctories bd exists."
else
	mkdir bd 
fi

