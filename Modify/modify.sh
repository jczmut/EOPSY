#!/bin/bash
# 'modify.sh' shell script for EOPSY course at WUT
# author: Julia Czmut

# global variables to later set to 1 when flags are enabled
lowercase=0   # corresponding to flag -l
uppercase=0   # flag -u
recursive=0   # flag -r
is_sed=0      # sed pattern -> */*/*/*
# flag -h doesn't need a variable, because when it's present, the script presents info and exits
iteration=1
main_dir=$PWD


uppercase_recursive () {
  cd $1

  # going through all files in current directory
  for el in *
    do

      if [ -f $el ]   # if the element is a file, we will uppercase its name
      then
        only_name="${el%.*}"  # extracting the part of the name without the extension

        if [ $el = $only_name ]   # if the file has no extension
        then
          rename 'y/a-z/A-Z/' $el   # we can use the 'rename' command without additional transformations

        else   # the file has an extension
          only_name=${only_name^^}            # uppercase the part of the name without extension
          extension="${el##*.}"               # get the extension
          full_name=$only_name.$extension     # join both parts to get the full name in proper form
          mv $el $full_name         # rename the file
        fi

      fi

      if [ -d $el ]   # if the current element is a directory - we go in deeper
      then
        uppercase_recursive $el
      fi

    done
  
  cd ..
}

lowercase_recursive () {
  cd $1

  # going through all files in current directory
  for el in *
    do

      if [ -f $el ]   # if the element is a file, we will lowercase its name
      then
        only_name="${el%.*}"  # extracting the part of the name without the extension

        if [ $el = $only_name ]   # if the file has no extension
        then
          rename 'y/A-Z/a-z/' $el   # we can use the 'rename' command without additional transformations

        else   # the file has an extension
          only_name=${only_name,,}            # lowercase the part of the name without extension
          extension="${el##*.}"               # get the extension
          full_name=$only_name.$extension     # join both parts to get the full name in proper form
          mv $el $full_name         # rename the file
        fi

      fi

      if [ -d $el ]   # if the current element is a directory - we go in deeper
      then
        lowercase_recursive $el
      fi
    done
  
  cd ..
}

sed_recursive () {
  cd $1

  # going through all files in current directory
  for el in *
    do

      if [ -f $el ]   # if the element is a file, we will change its name  - if it doesn't have an extension
      then

        only_name="${el%.*}"  # extracting the part of the name without the extension

        if [ $el = $only_name ]   # if the file has no extension
        then
          rename $pattern $el   # we can use the 'rename' command without additional transformations

        else   # the file has an extension

          only_name=$(echo $only_name | sed $pattern)           # transform the the part of the name without extension
          extension="${el##*.}"                                 # get the extension
          full_name=$only_name.$extension                        # join both parts to get the full name in proper form

          if [ "$el" != $full_name ]  # condition to prevent trying to change the file names which did not need changing
			    then
            mv $el $full_name         # rename the file
		      fi
      
        fi
      fi

      if [ -d $el ]   # if the current element is a directory - we go in deeper
      then
        sed_recursive $el
      fi

    done
    cd ..
}

usage () {    # function which prints the information about the script if -h is envoked, or an error

  echo -e "\n**************************** HELP ****************************\n\nThe script 'modify' is dedicated to lowerizing, uppercasing file names or modifying them using the sed command.\nChanges may be done either with recursion or without it."
  echo -e "\nOptions:\n\t-r\t\tenvokes recursion\n\t-l|-u\t\tlowercase or uppercase\n\t<sed pattern>\tsed pattern\n\t-h\t\tshows help\n"
  echo -e "Syntax options:\n\tmodify.sh [-l|-u] <file name>\n\tmodify.sh [-r] [-l|-u] <directory name>\n\tmodify.sh [-rl|-lr|-ru|-ur] <directory name>\n\tmodify.sh <sed pattern> <file name>\n\tmodify.sh [-r] <sed pattern> <directory name>\n\tmodify.sh [-h]\n"    

}


# ////////////////////////////////////////////////////////////// MAIN PART //////////////////////////////////////////////////////////////

# loop for shifting arguments
for arg in "$@"
do

  if [ $iteration = 2 ]     # iteration 2 - we need to shift by the adequate amount of arguments and make sure to retain the flags
  then
    n=1     # starting at 1, because we also have to skip the first file/directory
    # at the 2nd iteration, we are past all the checks, so we are sure there aren't too many flags envoked
    if [ $recursive = 1 ]
    then
      ((n++))
    fi
    if [ $uppercase = 1 ]
    then
      ((n++))
    fi
    if [ $lowercase = 1 ]
    then
      ((n++))
    fi
    if [ $is_sed = 1 ]
    then
      ((n++))
    fi

    shift $n

    if [ -z $1 ]    # no arguments left
    then
      exit
    fi

    if [ -d $1 ] || [ -f $1 ]
    then
      path=$1
      name="$(basename $1)"
    fi
  fi

  if [ $iteration -gt 2 ]   # iterations 3 and higher
  then
    shift 1

    if [ -z $1 ]    # no arguments left
    then
      exit
    fi

    if [ -d $1 ] || [ -f $1 ]
    then
      path=$1
      name="$(basename $1)"
    
    else    # so the argument we shifted to is neither a file nor a directory   
      echo -e "\nError: Wrong file/directory provided."
      usage
      exit
    fi

  fi

  # checking if there are any flags - if not, exit
  if [ -z $1 ]
  then
    echo -e "\nThere are no flags specified to the command."
    usage
    exit
  fi

  if [ $iteration = 1 ]     # we need to assign the global variables only at the first iteration
  then
    # checking flags by going through all of the arguments and setting the adequate global variables to 1
    # and saving the name of the file/directory
    for i in "$@"
    do

      if [ -z $name ]
      then

        if [ -d "${i}" ]
        then
          path=$i
          name="$(basename $i)"
        fi

        if [ -f "$i" ]
        then
          path=$i
          name="$(basename $i)"
        fi

      fi

      if [ -h = $i ]  # -h, no variable here - not needed
      then
        usage
        exit
      fi

      if [ -r = $i ]  # -r
      then
        recursive=1
        fi

      if [ -l = $i ]  # -l
      then
        lowercase=1
      fi

      if [ -u = $i ]  # -u
      then
        uppercase=1
      fi

      if [ -ru = $i ] || [ -ur = $i ]   # -ru or -ur
      then
        uppercase=1
        recursive=1
      fi

      if [ -lr = $i ] || [ -rl = $i ]   # -lr or -rl
      then
        lowercase=1
        recursive=1
      fi

      if [ -lu = $i ] || [ -ul = $i ]   # -lu or -ul (PROHIBITED, later checked)
      then
        lowercase=1
        uppercase=1
      fi

      if [[ $i = */*/*/* ]]   # sed pattern
      then 
        if [ ! -d $i ] && [ ! -f $i ]
        then
          is_sed=1
          pattern="$i"
        fi
      fi

    done

  fi
  
  if [ ! -z $path ]
  then
    cd $(dirname $path)
  fi

  # checking if provided file/directory exists
  if [ ! -d "$name" ] && [ ! -f "$name" ]
  then
    echo -e "\nError: This file/directory does not exist."
    usage
    exit
  fi

  # checking if the variables uppercase and lowercase are not both set to 1 - they cannot be
  if [ $uppercase = 1 ] && [ $lowercase = 1 ]
  then
    echo -e "\nError: Not possible to both lowerize and uppercase file names."
    usage
    exit
  fi

  # checking if sed variable is set to 1 - if it is, then lowercase and uppercase cannot be 1 at the same time
  if [ $is_sed = 1 ]
  then
    if [ $uppercase = 1 ] || [ $lowercase = 1 ]
    then
      echo -e "\nError: Not possible to use -l or -u option with the sed pattern."
      usage
      exit
    fi
  fi


  #************************************************************* UPPERCASE **********************************************************************************
  if [ $uppercase = 1 ]
  then

    if [ $recursive = 1 ]
      then

        if [ ! -d $name ]   # when recursive, a whole directory name must be given
          then
            echo -e "\nError: Not a directory."
            usage
            exit
        fi

        uppercase_recursive $name  # go to the function dedicated to recursively changing all files within the directory
        cd $main_dir

    else  # not recursive

      if [ ! -f $name ]   # when not recursive, a file name must be given
      then
        echo -e "\nError: Not a file."
        usage
        exit
      fi

      only_name="${name%.*}"    # name without the extension

      if [ $name = $only_name ]   # checking whether the file name doesn't have an extension and 'rename' command can be used without further transformations
      then
        rename 'y/a-z/A-Z/' $name
      else
        # the file has an extension, so we have to make sure to uppercase only the part without the extension (only_name variable)
        only_name=${only_name^^}
        extension="${name##*.}"
        full_name=$only_name.$extension
        mv $name $full_name
      fi

    fi
  fi

  #************************************************************* LOWERCASE **********************************************************************************
  if [ $lowercase = 1 ]
  then

    if [ $recursive = 1 ]
      then

        if [ ! -d $name ]   # when recursive, a whole directory name must be given
          then
            echo -e "\nError: not a directory."
            usage
            exit
        fi

        lowercase_recursive $name  # go to the function dedicated to recursively changing all files within the directory
        cd $main_dir

    else  # not recursive

      if [ ! -f $name ]   # when not recursive, a file name must be given
      then
        echo -e "\nError: Not a file."
        usage
        exit
      fi
      
      only_name="${name%.*}"    # name without the extension

      if [ $name = $only_name ]   # checking whether the file name doesn't have an extension and 'rename' command can be used without further transformations
      then
        rename 'y/A-Z/a-z/' $name

      else
        # the file has an extension, so we have to make sure to uppercase only the part without the extension (only_name variable)
        only_name=${only_name,,}
        extension="${name##*.}"
        full_name=$only_name.$extension
        mv $name $full_name
      fi

    fi
  fi

  #************************************************************* SED PATTERN **********************************************************************************
  if [ $is_sed = 1 ]
  then

      if [ $recursive = 1 ]
      then
          if [ ! -d $name ]   # when recursive, a whole directory name must be given
          then
            echo -e "\nError: not a directory."
            usage
            exit
          fi

        sed_recursive $name
        cd $main_dir

      else    # not recursive

        if [ ! -f $name ]   # when not recursive, a file name must be given
        then
          echo -e "\nError: Not a file."
          usage
          exit
        fi

        only_name="${name%.*}"    # name without the extension

        if [ $name = $only_name ]   # checking whether the file name doesn't have an extension and 'rename' command can be used without further transformations
        then
          rename $pattern $name

        else
          # the file has an extension, so we have to make sure not to change the extension
          only_name=$(echo $only_name | sed $pattern)
          extension="${name##*.}"
          full_name=$only_name.$extension
          mv $name $full_name
        fi

      fi
  fi

  cd -    # go back to previous location
  ((iteration++))

done
