#!/bin/bash

source ~/.bashrc

## declare an array variable
#declare -a arr=("Bass")
#declare -a arr=("Bass" "Wu")
#declare -a arr=("Bass" "Wu" "Sanchez" "Hawkins")
#declare -a arr=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson")
#declare -a arr=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays")
#declare -a arr=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays" "Keith" "Chandler" "Paul" "Choi" "Key" "Shah" "Wong" "Ballard" "Duke" "Brady" "Miles" "Davidson" "Huber" "Pierce" "Pittman" "Avila")
#declare -a arr=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays" "Keith" "Chandler" "Paul" "Choi" "Key" "Shah" "Wong" "Ballard" "Duke" "Brady" "Miles" "Davidson" "Huber" "Pierce" "Pittman" "Avila" "Randall" "Holmes" "Wiley" "Harvey" "Terrell" "Ward" "Krueger" "Bean" "Stein" "Moon" "Le" "Ochoa" "Moyer" "Mcclure" "Walters" "Mays" "Downs" "Harding" "Merritt" "Mcgee" "Lynn" "Short" "Kemp" "Oneill" "Erickson" "Barber" "Carrillo" "Duncan" "Morales" "Reid" "Noble" "Bernard")
declare -a arr=("Bass" "Wu" "Sanchez" "Hawkins" "Valenzuela" "Hensley" "Bailey" "Carson" "Carr" "Hunt" "Stanley" "Nash" "Ibarra" "Gould" "Orr" "Hays" "Keith" "Chandler" "Paul" "Choi" "Key" "Shah" "Wong" "Ballard" "Duke" "Brady" "Miles" "Davidson" "Huber" "Pierce" "Pittman" "Avila" "Randall" "Holmes" "Wiley" "Harvey" "Terrell" "Ward" "Krueger" "Bean" "Stein" "Moon" "Le" "Ochoa" "Moyer" "Mcclure" "Walters" "Mays" "Downs" "Harding" "Merritt" "Mcgee" "Lynn" "Short" "Kemp" "Oneill" "Erickson" "Barber" "Carrillo" "Duncan" "Morales" "Reid" "Noble" "Bernard" "Brewer" "Vega" "Campos" "Jones" "Marquez" "Frederick" "Watts" "Benjamin" "Rodriguez" "Barr" "Luna" "Coleman" "Kramer" "Lopez" "Bentley" "Macias" "Evans" "Salinas" "Copeland" "Payne" "Watkins" "Mcgrath" "Mack" "Carpenter" "Roach" "Harrell" "Navarro" "James" "Simmons" "Haley" "Zuniga" "Page" "Hurst" "Donovan" "Patterson" "Cervantes" "Doyle" "Montoya" "Velez" "Yu" "May" "Berger" "Day" "Dominguez" "West" "Wilkerson" "Reeves" "Mcguire" "Ellison" "Munoz" "Jefferson" "Bautista" "Gardner" "Dennis" "Wall" "Morse" "Cook" "Mccullough" "Duran" "Thomas" "Meyers" "Wright" "Daugherty" "Barker")

lua coordinator.lua &

## now loop through the above array
for i in "${arr[@]}"
do
   echo "$i"
   lua client.lua "$i" $RANDOM &
done

wait