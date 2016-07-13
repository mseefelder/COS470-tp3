#!/bin/bash

source ~/.bashrc

## declare an array variable

declare -a arr128=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays" "Keith" "Chandler" "Paul" "Choi" "Key" "Shah" "Wong" "Ballard" "Duke" "Brady" "Miles" "Davidson" "Huber" "Pierce" "Pittman" "Avila" "Randall" "Holmes" "Wiley" "Harvey" "Terrell" "Ward" "Krueger" "Bean" "Stein" "Moon" "Le" "Ochoa" "Moyer" "Mcclure" "Walters" "Mays" "Downs" "Harding" "Merritt" "Mcgee" "Lynn" "Short" "Kemp" "Oneill" "Erickson" "Barber" "Carrillo" "Duncan" "Morales" "Reid" "Noble" "Bernard" "Brewer" "Vega" "Campos" "Jones" "Marquez" "Frederick" "Watts" "Benjamin" "Rodriguez" "Barr" "Luna" "Coleman" "Kramer" "Lopez" "Bentley" "Macias" "Evans" "Salinas" "Copeland" "Payne" "Watkins" "Mcgrath" "Mack" "Carpenter" "Roach" "Harrell" "Navarro" "James" "Simmons" "Haley" "Zuniga" "Page" "Hurst" "Donovan" "Patterson" "Cervantes" "Doyle" "Montoya" "Velez" "Yu" "May" "Berger" "Day" "Dominguez" "West" "Wilkerson" "Reeves" "Mcguire" "Ellison" "Munoz" "Jefferson" "Bautista" "Gardner" "Dennis" "Wall" "Morse" "Cook" "Mccullough" "Duran" "Thomas" "Meyers" "Wright" "Daugherty" "Barker")

lua coordinator.lua &

## now loop through the above array
for i in "${arr128[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log128.txt
mv critical.txt critical128.txt

declare -a arr64=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays" "Keith" "Chandler" "Paul" "Choi" "Key" "Shah" "Wong" "Ballard" "Duke" "Brady" "Miles" "Davidson" "Huber" "Pierce" "Pittman" "Avila" "Randall" "Holmes" "Wiley" "Harvey" "Terrell" "Ward" "Krueger" "Bean" "Stein" "Moon" "Le" "Ochoa" "Moyer" "Mcclure" "Walters" "Mays" "Downs" "Harding" "Merritt" "Mcgee" "Lynn" "Short" "Kemp" "Oneill" "Erickson" "Barber" "Carrillo" "Duncan" "Morales" "Reid" "Noble" "Bernard")

lua coordinator.lua &

## now loop through the above array
for i in "${arr64[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log64.txt
mv critical.txt critical64.txt

declare -a arr32=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays" "Keith" "Chandler" "Paul" "Choi" "Key" "Shah" "Wong" "Ballard" "Duke" "Brady" "Miles" "Davidson" "Huber" "Pierce" "Pittman" "Avila")

lua coordinator.lua &

## now loop through the above array
for i in "${arr32[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log32.txt
mv critical.txt critical32.txt

declare -a arr16=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays")

lua coordinator.lua &

## now loop through the above array
for i in "${arr16[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log16.txt
mv critical.txt critical16.txt

declare -a arr8=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson")

lua coordinator.lua &

## now loop through the above array
for i in "${arr8[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log8.txt
mv critical.txt critical8.txt

declare -a arr4=("Bass" "Wu" "Sanchez" "Hawkins")

lua coordinator.lua &

## now loop through the above array
for i in "${arr4[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log4.txt
mv critical.txt critical4.txt

declare -a arr2=("Bass" "Wu")

lua coordinator.lua &

## now loop through the above array
for i in "${arr2[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log2.txt
mv critical.txt critical2.txt

declare -a arr1=("Bass")

lua coordinator.lua &

## now loop through the above array
for i in "${arr1[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
   sleep 1
done

wait

mv log.txt log1.txt
mv critical.txt critical1.txt
