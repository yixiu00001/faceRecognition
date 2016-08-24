awk -F " " 'BEGIN{
	ORS="\t"
	sum=0
	flag=0
	#allsum=4805670
	#allsum=260544000
	#allsum=3922450
	#allsum=214277000
	#allsum=6519430
	allsum=359151000
}{

	if($1>=50)
	{
		sum+=$1
	}else if($1>=49)
	{
		if(flag==0)
		{
			printf(">=50  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}
	else if($1>=48)
	{
		if(flag==1)
		{
			printf(">=49  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=47)
	{
		if(flag==0)
		{
			printf(">=48  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=46)
	{
		if(flag==1)
		{
			printf(">=47  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=45)
	{
		if(flag==0)
		{
			printf(">=46  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=44)
	{
		if(flag==1)
		{
			printf(">=45  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=43)
	{
		if(flag==0)
		{
			printf(">=44  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=42)
	{
		if(flag==1)
		{
			printf(">=43  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=41)
	{
		if(flag==0)
		{
			printf(">=42  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=40)
	{
		if(flag==1)
		{
			printf(">=41  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=41)
	{
		if(flag==0)
		{
			printf(">=42  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=40)
	{
		if(flag==1)
		{
			printf(">=41  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=39)
	{
		if(flag==0)
		{
			printf(">=40  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=38)
	{
		if(flag==1)
		{
			printf(">=39  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=37)
	{
		if(flag==0)
		{
			printf(">=38  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=36)
	{
		if(flag==1)
		{
			printf(">=37  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=35)
	{
		if(flag==0)
		{
			printf(">=36  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=34)
	{
		if(flag==1)
		{
			printf(">=35  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=33)
	{
		if(flag==0)
		{
			printf(">=34  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=32)
	{
		if(flag==1)
		{
			printf(">=33  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=31)
	{
		if(flag==0)
		{
			printf(">=32  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}
		sum+=$1
	}else if($1>=30)
	{
		if(flag==1)
		{
			printf(">=31  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=29)
	{
		if(flag==0)
		{
			printf(">=30  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=28)
	{
		if(flag==1)
		{
			printf(">=29  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=27)
	{
		if(flag==0)
		{
			printf(">=28  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=26)
	{
		if(flag==1)
		{
			printf(">=27  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=25)
	{
		if(flag==0)
		{
			printf(">=26  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=24)
	{
		if(flag==1)
		{
			printf(">=25  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=23)
	{
		if(flag==0)
		{
			printf(">=24  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=22)
	{
		if(flag==1)
		{
			printf(">=23  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=21)
	{
		if(flag==0)
		{
			printf(">=22  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=20)
	{
		if(flag==1)
		{
			printf(">=21  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=19)
	{
		if(flag==0)
		{
			printf(">=20  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=18)
	{
		if(flag==1)
		{
			printf(">=19  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=17)
	{
		if(flag==0)
		{
			printf(">=18  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=16)
	{
		if(flag==1)
		{
			printf(">=17  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=15)
	{
		if(flag==0)
		{
			printf(">=16  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else if($1>=14)
	{
		if(flag==1)
		{
			printf(">=15  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=0
		}
		sum+=$1
	}else if($1>=13)
	{
		if(flag==0)
		{
			printf(">=14  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
			
		}
		sum+=$1
	}else{
			printf(">=13  %15f  %15f\n",sum, sum*1.0/allsum)
			sum=0
			flag=1
		}



}'
