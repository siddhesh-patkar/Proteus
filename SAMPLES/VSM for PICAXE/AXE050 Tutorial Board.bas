

main:	readadc 2,b1
	if b1 > 150 then show9
	if b1 > 130 then show8
	if b1 > 110 then show7
	if b1 > 90 then show6
	if b1 > 70 then show5
	if b1 > 50 then show4
	if b1 > 30 then show3
	if b1 > 20 then show2
	if b1 > 10 then show1
	
show0:
	let pins = %11111100	' digit 0
	goto main
show1:
	let pins = %01100000	' digit 1
	goto main
show2:
	let pins = %11011010	' digit 2
	goto main
show3:
	let pins = %11110010	' digit 3
	goto main
show4:
	let pins = %01100110	' digit 4
	goto main
show5:
	let pins = %10110110	' digit 5
	goto main
show6:
	let pins = %10111110	' digit 6
	goto main
show7:
	let pins = %11100000	' digit 7
	goto main
show8:
	let pins = %11111110	' digit 8
	goto main
show9:
	let pins = %11110110	' digit 9
	goto main

