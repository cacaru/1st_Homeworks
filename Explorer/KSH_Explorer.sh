#!/bin/bash

cd ~

mkdir 2015726091-TrashBin

Form()
{
echo "=============================================== 2015726091 Sunho Kwon =================================================="
echo "======================================================== List =========================================================="
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "|                              |                                                |                                      |"
echo "==================================================== Information ======================================================="
echo "|                                                                                                                      |"
echo "|                                                                                                                      |"
echo "|                                                                                                                      |"
echo "|                                                                                                                      |"
echo "|                                                                                                                      |"
echo "|                                                                                                                      |"
echo "======================================================= Total =========================================================="
echo "|                                                                                                                      |"
echo "======================================================== END ==========================================================="
Total_file_infomation
}



#--------------------------file list _-------------------------------------
declare -a list
declare -a slist
declare -a rlist
declare -a size_ls
declare -a dlist
declare -a dslist
declare -a dsslist
declare -a selist
declare -a seselist
declare -a sesedlist
declare -a check
declare -a open_counter

declare -i i=1
declare -i k=0

declare -i cursor_count=0
declare -i cs=-1

File_listing()
{
	list=()
	slist=()
    rlist=()
    size_ls=()
    list[0]="[94m..[0m"
    slist[0]=".."
    rlist[0]=" "
    size_ls[0]=" "
	i=1
	for element_ls in `ls | grep *"TrashBin"; ls -1 | grep -v *"TrashBin"`
	do
		list[$i]="$element_ls"
		i=`expr $i + 1`
	done
	i=1
	for second_element in `ls | grep *"TrashBin"; ls -1 | grep -v *"TrashBin"`
	do
		slist[$i]="$second_element"
		i=`expr $i + 1`
	done
	i=1
    for ele in `ls --time-style='+%b %d %H:%M:%S %Y' -l | grep *"TrashBin" | cut -c 31-51; ls --time-style='+%b %d %H:%M:%S %Y' -l -1 | grep -v *"TrashBin" | cut -c 31-51`
    do
        rlist[$i]="$ele"
        i=`expr $i + 1`
    done
    i=1
    for ele in `ls -sh1 | grep -v "합계" | awk '{print $1}'`
    do
        size_ls[$i]="$ele"
        i=`expr $i + 1`
    done

}


File_time()
{
    i=$cursor_count
    j=1
    if [ $i == 0 ]
    then
        echo -n " "
   
    else
        i=`expr $i \* 4`
        i=`expr $i - 3`
        until [ `expr $j % 5` == 0 ]
        do
            if [ `expr $j % 5` == 1 ]
            then
                if [ ${rlist[$i]} == "1월" ] 
                then
                    rlist[$i]="January"
                elif [ ${rlist[$i]} == "2월" ]
                then
                    rlist[$i]="February"
                elif [ ${rlist[$i]} == "3월" ]
                then
                    rlist[$i]="March"
                elif [ ${rlist[$i]} == "4월" ]
                then
                    rlist[$i]="April"
                elif [ ${rlist[$i]} == "5월" ]
                then
                    rlist[$i]="May"
                elif [ ${rlist[$i]} == "6월" ]
                then
                    rlist[$i]="June"
                elif [ ${rlist[$i]} == "7월" ]
                then
                    rlist[$i]="July"
                elif [ ${rlist[$i]} == "8월" ]
                then
                    rlist[$i]="August"
                elif [ ${rlist[$i]} == "9월" ]
                then
                    rlist[$i]="Semtember"
                elif [ ${rlist[$i]} == "10월" ]
                then
                    rlist[$i]="October"
                elif [ ${rlist[$i]} == "11월" ]
                then
                    rlist[$i]="November"
                elif [ ${rlist[$i]} == "12월" ]
                then
                    rlist[$i]="December"
                fi
            fi
            echo -n "${rlist[$i]} "
            i=`expr $i + 1`
            j=`expr $j + 1`
        done
    fi
}

File_infomation()
{
    
	tput cup 31 2
	echo -n "File name :                                                                         "
	tput cup 31 14
	stat -c "%n" "${slist[$cursor_count]}" | cut -c 1-104
	tput cup 32 1
	File_type_
	tput cup 33 2 
	echo -n "File size :                                                                         "
	tput cup 33 14
	echo -n ${size_ls[$cursor_count]}
	tput cup 34 2
	echo -n "Access time :                                                                     "
	tput cup 34 16
	File_time
	tput cup 35 2
	echo -n "permission :                                                                        " 
	tput cup 35 14
	stat -c "%a" "${slist[$cursor_count]}" | cut -c 1-104
	tput cup 36 2
	echo -n "absolate path :                                                                     "
	tput cup 36 17
	echo -n ~+ "/${slist[$cursor_count]}" | cut -c 1-104
}

File_type_()
{
	if [ -d ${slist[$cursor_count]} ]
	then
        if [ ${slist[$cursor_count]} = "2015726091-TrashBin" ]
        then
            echo -e " File type :""[33m"" TrashBin         ""[0m"
        else        
		    echo -e " File type :""\e[34m"" directory       ""\e[0m"
        fi
	elif [  -x  ${slist[$cursor_count]}  ]
	then 
		echo -e " File type :""[32m"" execute file           ""[0m"
	else
	case "${slist[$cursor_count]}"
	in
	    *.zip | *.gz ) echo -e " File type : ""\e[31m""compressed file                ""\e[0m";;
		*) echo -e " File type :""\e[0m"" regular file           ""\e[0m";;
	esac
	fi
}

enter_what()
{
	if [ -d ${slist[$cursor_count]} ]
	then 
		cd ${slist[$cursor_count]}
		cursor_count=0
        x=2
		File_listing
		clear
		Form
	elif [ -r ${slist[$cursor_count]} ]
	then
		case "${slist[$cursor_count]}"
		in
			*.txt | *.sh | *.c | *.h ) clear
						Form
						File_print;; 
			*.o | *.out ) ;;
			*) ;;
		esac
	fi
}

File_print()
{
	i=1
	j=2
	while read line
	do
		if [ $i = 29 ]
		then
			break
		fi
		tput cup $j 32
		echo "$i" $line | cut -c 1-48
		i=`expr $i + 1`
		j=`expr $j + 1`
	done < ${slist[$cursor_count]}
	
}

File_del()
{
    now_dir=`pwd | cut -f 1-3 -d /`
    if [ $now_dir == "2015726091-TrashBin" ]
    then
        rm -rf ${slist[$cursor_count]}
        base

    else
        if [ -d ${slist[$cursor_count]} ]
        then
            tar -cf a.tar ${slist[$cursor_count]}
            tar -xf a.tar -C $now_dir/2015726091-TrashBin/
            rm -rf a.tar
            rm -rf ${slist[$cursor_count]}
        elif [ -f ${slist[$cursor_count]} ]
        then
            cat ${slist[$cursor_count]} > $now_dir/2015726091-TrashBin/${slist[$cursor_count]}
            rm -rf ${slist[$cursor_count]}
        fi
    fi
    base
}

File_tree()
{
    if [ -d ${slist[$cursor_count]} ]
    then   
        xx=3
        yy=80
    
        tput cup $xx $yy
        cc=1
        cursor_mv 
    else
        :
    fi
}

cursor_mv()
{
    new_listing
    open_count
    show_tree
    while [ 1 ]
    do
        tput civis
        read -sn 1 key
        if [ "$key" = '' ]
        then
            read -sn 1 key
            read -sn 1 key
            if [ "$key" = 'A' ]
            then
                if [ $xx -ge 4 ]
                then
                    xx=`expr $xx - 1`
                    cc=`expr $cc - 1`
                    tput cup $xx $yy
                    xxx=2
                    show_tree
                else
                    tput cup $xx $yy
                fi
            elif [ "$key" = 'B' ]
            then
                if [ $xx -le 28 ] && [ $xx -le ${#dlist[*]} ]
                then
                    xx=`expr $xx + 1`
                    cc=`expr $cc + 1`
                    tput cup $xx $yy
                    xxx=2
                    show_tree
                fi
            fi
        elif [ "$key" = "" ]
        then
            if [ -d ${dsslist[$cc]} ]
            then
                cs=$cc
                xxx=2
                sec_listing
                show_tree
            else
                :
            fi
        elif [ "$key" = 'r' ]
        then
            open_count
            hi=1
            cd $dir
            base
            tput cup $x $y
            break
        else
            :
        fi
       
    done
}

open_count()
{
    i=`expr ${#dlist[*]} - 1`                # 열었었는지 아닌지 검사하기 위한 리스트
    j=0
    until [ $j -eq $i ]
    do
        open_counter[$j]="5"
        j=`expr $j + 1`
    done
}

new_listing()
{
    dir=`pwd`
    cd ${slist[$cursor_count]}    
    ddir=`pwd`
    dlist=()
    dslist=()
    llist=()
    dlist[0]=${slist[$cursor_count]}   #출력용
    i=1
    for ele in `ls --color -1`
    do
        dlist[$i]="$ele"
        i=`expr $i + 1`
    done

    dslist[0]=${slist[$cursor_count]}  #이름 검사용
    i=1
    for ele in `ls -1`
    do
        dslist[$i]="$ele"
        i=`expr $i + 1`
    done
    dsslist[0]="$dir/${slist[$cursor_count]}"   #디렉토리 이동용
    i=1
    for ele in `ls -1`
    do
        dsslist[$i]="$ddir/$ele"
        i=`expr $i + 1`
    done
    cd ..
}

sec_listing()
{
    selist=()
    seselist=()
    i=0
    cd ${dsslist[$cc]}

    for ele in `ls --color -1`
    do
        selist[$i]="$ele"   #하위 디렉토리 출력저장용
        i=`expr $i + 1`
    done
    i=0
    for ele in `ls -1`
    do
        seselist[$i]="$ele" #하위 디렉토리 이름 검사용
    done
  
    i=0
    for ele in `ls -1`
    do
        sesedlist[$i]="${dsslist[$cc]}/$ele" # 하위 디렉토리 파일 경로 저장 - 이후 디렉토리로도 이동가능하게
        i=`expr $i + 1`
    done

}

new_list_print()
{
    tput cup $xxx $yy
    j=0
    until [ $j = ${#dlist[*]} ]
    do
        if [ $j -gt 27 ]
        then
            :
        elif [ $j -eq 0 ]
        then
            echo -e "|""[34m"${dlist[$j]}"[0m"
            xxx=`expr $xxx + 1`
            j=`expr $j + 1`
            continue
        elif [ "$cc" -eq "$j" ]
        then
                tput cup $xxx $yy
                echo -e "|""[7m""-"${dlist[$j]}"[27m""                                              "   | cut -c 1-47
        else
            tput cup $xxx $yy 
            echo -ne "|-"${dlist[$j]} "                                                                  "  | cut -c 1-39 
        fi
        j=`expr $j + 1`
        xxx=`expr $xxx + 1`
    done

    i=${#dlist[*]}
    if [ $i -le 28 ]
    then
        until [ $i -eq 28 ]
        do
            tput cup $xxx $yy
            echo "|                                      "
            i=`expr $i + 1`
            xxx=`expr $xxx + 1`
        done
    else
        :
    fi
}

hip()
{
    if [ $hi -eq 1 ]
    then
        hipen="--"
    elif [ $hi -eq 2 ]
    then
        hipen="----"
    elif [ $hi -eq 3 ]
    then
        hipen="------"
    elif [ $hi -eq 4 ]
    then
        hipen="--------"
    elif [ $hi -eq 5 ]
    then
        hipen="----------"
    elif [ $hi -eq 6 ]
    then
        hipen="------------"
    fi
}

list_move()
{
    pwdl=`pwd | cut -f 5 -d /`
    pwdll=`pwd | cut -f 6 -d /`
    pwdlll=`pwd | cut -f 7 -d /`
    pwdllll=`pwd | cut -f 8 -d /`
    pwdlllll=`pwd | cut -f 9 -d /`
    open_counter[$cc]=`expr ${open_counter[$cc]} - 3`
    if [[ $pwdl == ${dslist[$cc]} ]]
    then
        hi="1"
    
    elif [[ $pwdll == ${dslist[$cc]} ]]
    then
        hi="2"

    elif [[ $pwdlll == ${dslist[$cc]} ]]
    then
        hi="3"
        
    elif [[ $pwdllll == ${dslist[$cc]} ]]
    then
        hi="4"
        
    elif [[ $pwdlllll == ${dslist[$cc]} ]]
    then
        hi="5"
        
    else
        echo $pwdl
        hi="6"
    fi
    hip
    i=`expr ${#selist[*]} - 1`
    j=0
    k=${#dlist[*]}
    until [ $j -eq $i ]
    do
        dlist[$k]=" "
        dslist[$k]=" "
        dsslist[$k]=" "
        open_counter[$k]=" "
        k=`expr $k + 1`
        j=`expr $j + 1`
    done

    i=`expr ${#dlist[*]} - ${#selist[*]}`
    j=${#dlist[*]}
    k=$cc
    until [ $j -eq $k ]
    do
        dlist[$j]="${dlist[$i]}"            
        dslist[$j]="${dslist[$i]}"
        dsslist[$j]="${dsslist[$i]}"
        open_counter[$j]="${open_counter[$i]}"
        i=`expr $i - 1`
        j=`expr $j - 1`
    done

    i=0
    k=`expr $cc + 1`
    until [ $i -eq ${#selist[*]} ]
    do        
        dlist[$k]="$hipen ${selist[$i]}"
        dslist[$k]="${seselist[$i]}"
        dsslist[$k]="${sesedlist[$i]}"
        open_counter[$k]="5"
        i=`expr $i + 1`
        k=`expr $k + 1`
    done
    cs=-1
    

}

list_back()
{
    check=()
    cd ${dsslist[$cc]}
    i=0
    for ele in `ls -1`
    do
        check[$i]="$ele"
        i=`expr $i + 1`
    done
    cd ..
    ddir=`pwd`

    i=`expr $cc + ${#check[*]} + 1`
    j=${#dlist[*]}
    k=`expr $cc + 1`
    until [ $i -eq $j ]
    do
        dlist[$k]="${dlist[$i]}"
        dslist[$k]="${dslist[$i]}"
        dsslist[$k]="${dsslist[$i]}"
        open_counter[$k]="${open_counter[$i]}"
        i=`expr $i + 1`
        k=`expr $k + 1`
    done

    i=${#dlist[*]}
    j=`expr ${#dlist[*]} - ${#check[*]}`
    until [ $j -eq $i ]
    do
        unset dlist[$j]
        unset dslist[$j]
        unset dsslist[$j]
        unset open_counter[$j]
        j=`expr $j + 1`
    done
    hi=`expr $hi - 1`
    open_counter[$cc]=`expr ${open_counter[$cc]} + 3`
    cs=-1
}


show_tree()
{
    xxx=2
    if [ $cs -eq $cc ]
    then
        if [ ${open_counter[$cc]} -eq 5 ]
        then
            if [ ${#selist[*]} -eq 0 ]
            then
                new_list_print
            else
                list_move
                new_list_print
            fi
        else
            list_back
            new_list_print
        fi
    else
        new_list_print  
    fi
 
}

#-------------------------cursor input---------------------------------------
declare -i x=2
declare -i y=1
declare -i xx=2
declare -i yy=80
declare -i xxx=2
declare -i hi=0
declare -i openc=1
declare -i cc
cursor_input()
{
    File_listing
	File_infomation
	tput cup 2 1
	while [ ture ]
	do
		File_list
		tput civis
		read -s -n 1 key
        if [ "$key" = '' ]
        then
            read -s -n 1 key
            read -s -n 1 key
            if [ "$key" = 'A' ]
            then
                if [ $x -ge 3 ]
                then
                    x=`expr $x - 1`
                    cursor_count=`expr $cursor_count - 1`
                    base
                    tput cup $x $y
                fi

            elif [ "$key" = 'B' ]
            then
                if [ $x -le 28 ] && [ $x -le ${#list[*]} ]
                then
                    x=`expr $x + 1`
                    cursor_count=`expr $cursor_count + 1`
                    base
                    tput cup $x $y
                fi
            fi
        elif [ "$key" = "" ] 
        then
            enter_what
            File_listing
            File_infomation
        elif [ "$key" = 'd' ]
        then
        
            File_del
        elif [ "$key" = 't' ]
        then
            File_tree
        elif [ "$key" = 'q' ]
        then
            break
        fi

    done
}

File_list()
{
    tput cup 2 0
	j=0
	until [ $j = ${#list[*]} ]
	do
        if [ $j != 0 ]
        then
            zip=`ls ${list[$j]} | grep '.zip$'`
            gz=`ls ${list[$j]} | grep '.gz$'`
        fi

		if [ $j -gt 28 ]
		then 
			:
        elif [ $j -eq 0 ] && [ $cursor_count -eq 0 ]
        then
            echo -e "|""[7m"${list[$j]} "[0m"   | cut -c 1-30
            j=`expr $j + 1`
            continue
        elif [ $cursor_count = $j ]
        then
            if [ -d ${list[$j]} ]
            then
                if [ ${list[$j]} == "2015726091-TrashBin" ]
                then
                    echo -e "|""[43m[39m"${list[$j]}"[0m"
                else
                    echo -e "|""[104m[30m"${list[$j]}"[0m"   
                fi
            elif [ -f ${list[$j]} ]
            then
                if [ -x ${list[$j]} ]
                then
                    echo -e "|""[102m[30m"${list[$j]}"[0m"             
                elif [ $zip ] || [ $gz ]
                then
                    echo -e "|""[101m"${list[$j]}"[0m"
                else    
                    echo -e "|""[7m"${list[$j]}"[27m"   
                fi
            else
                echo -e "|""[7m"${list[$j]}"[27m"  
            fi
        else
            if [ -d ${list[$j]} ]
            then
                if [ ${list[$j]} == "2015726091-TrashBin" ]
                then
                    echo -e "|""[33m"${list[$j]}"[0m"
                else
                    echo -e "|""[34m"${list[$j]}"[0m"   | cut -c 1-35
                fi
            elif [ -f ${list[$j]} ]
            then
                if [ -x ${list[$j]} ]
                then
                    echo -e "|""[92m"${list[$j]}"[0m"   | cut -c 1-35
                elif  [ $zip ] || [ $gz ]
                then
                    echo -e "|""[91m"${list[$j]}"[0m"   | cut -c 1-35
                else
                    echo "|"${list[$j]} | cut -c 1-30
                fi
            else
                echo "|"${list[$j]} | cut -c 1-30
            fi
        fi
		j=`expr $j + 1`
	done
}


#-------------------------total file infomation----------------------------------

Total_file_infomation()
{
	tput cup 38 2
	total=`find . -maxdepth 1 \! -name ".*" | wc -l`
	dir=`find . -maxdepth 1 -type d \! -name ".*" | wc -l`
	file=`find . -maxdepth 1 -type f \! -name ".*" | wc -l`
    zip_n=`ls | grep '.zip$' | wc -l`
    gz_n=`ls | grep '.gz$' | wc -l`
    ex_n=`ls -F | grep '*$' | wc -l`
	sfile=`expr $zip_n + $gz_n + $ex_n`
	
	file=`expr $file - $sfile`
	byte_size=`du -sSh | awk '{print $1}'`
	echo -ne " \t\t\t\t Total : "$total", Directory: "$dir", SFile:" $sfile", NFile :"$file", Size: "$byte_size
	echo
}

base()
{
    clear
    Form    
    File_listing
    File_infomation
}


clear
Form
cursor_input
