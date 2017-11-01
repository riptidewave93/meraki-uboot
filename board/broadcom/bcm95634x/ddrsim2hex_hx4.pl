my $in = $ARGV[0];
my $speed = $ARGV[1];
my $out = $in.".array";

my $str, $array1, $array2, $num, $num2, $i, $byte1, $byte2;
my $list = ",14,36,37,38,39,40,42,61,78,79,80,81,83,84,85,86,87,88,112,116,117,119,120,121,122,123,130,139,148,149,150,152,153,156,157,158,160,161,162,163,165,166,168,170,171,175,176,180,181,182,185,187,194,205,";
$prev_lnum = 0;
    open(my $F, "<", "$in") or die "can't open in file";
    open(my $FF, ">", "$out") or die "can't open in file";
    $array1 = "array1 = { \n";
    $array2 = "ddr3_init_tab_".$speed."[] = { \n";
    while($str = <$F>) {
       $str =~ m/_CTL_(.*)_DATA.adr, 32'h(.*), 32'h/;
      if( $1 && $2) {
       $num = $1;
       $val = $2;
       $num2 = $num;
       if($num2 == "01" || $num2 == "02" || $num2 == "03" || $num2 == "04" || $num2 == "05" || $num2 == "06" || $num2 == "07" || $num2 == "08" || $num2 == "09") {
		$num2 =~/0(.*)/;
			$num = $1;
	   }
       $byte1 = ",".$num.",";
       
       if($list =~ m/$byte1/) {
			$array1 .= "\t".$num.", 0x".$val.",\n";
		}
		else {
			$array2 .= "\t".$num.", 0x".$val.",\n";		
		}
	 }	
	}
	$array1 .= "\t0xffffffff\n};\n\n";
	$array2 .= "\t0xffffffff\n};\n";
#	print $array1;
#	print $array2;       
	print $FF $array1;
	print $FF $array2;
   
close($F);

close($FF);
