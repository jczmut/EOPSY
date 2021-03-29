#!/bin/bash

# modify [-r] [-l|-u] <dir/file name>
# modify [-r] <sed pattern> <dir/file name>
# modify [-h]

# global variables to later set to 1 when flags are enabled
lowercase=0   # corresponding to flag -l
uppercase=0   # flag -u
recursive=0   # flag -r
is_sed=0      # sed pattern -> */*/*/*
# flag -h doesn't need a variable, because when it's present, the script presents info and exits


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
  rename 'y/A-Z/a-z/' $1    # changing the directory name back to lowercase
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


# ////////////////////////////////////////////////////////////// MAIN PART //////////////////////////////////////////////////////////////


# checking if there are any flags - if not, exit
if [ -z $1 ]
then
  echo "There are no flags specified to the command."
  exit
fi


# checking flags by going through all of the arguments and setting the adequate global variables to 1
# and saving the name of the file/directory
for i in "$@"
do

  if [ -d "${i}" ]
  then
    name="$i"
  fi

  if [ -f "$i" ]
  then
    name="$i"
  fi

  if [ -h = $i ]  # -h, no variable here - not needed
  then
    echo -e "The script 'modify' is dedicated to lowerizing (-l) file names, uppercasing (-u) file names,\nor internally calling sed command with the given sed pattern which will operate on file names.\nChanges may be done either with recursion (-r) or without it."
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

  if [[ $i = */*/*/* ]]   # sed pattern
  then 
    is_sed=1
    pattern="$i"
  fi

done

echo uppercase value: $uppercase
echo lowercase value: $lowercase
echo recursive value: $recursive
echo sed pattern value: $is_sed
echo name of the file/directory: $name

# checking if there weren't too many arguments provided
if [ $recursive = 1 ]
then
  if [ "$#" -gt 3 ]
  then
    echo "Error: Too many parameters provided (>3)."
    exit
  fi
fi
if [ $recursive = 0 ]
  then
  if [ "$#" -gt 2 ]
  then
    echo "Error: Too many parameters provided (>2)."
    exit
  fi
fi

# checking if provided file/directory exists
if [ ! -d "$name" ] && [ ! -f "$name" ]
then
  echo "Error: This file/directory does not exist."
  exit
fi

# checking if the variables uppercase and lowercase are not both set to 1 - they cannot be
if [ $uppercase = 1 ] && [ $lowercase = 1 ]
then
  echo "Error: Not possible to both lowerize and uppercase file names."
  exit
fi

# checking if sed variable is set to 1 - if it is, then lowercase and uppercase cannot be 1 at the same time
if [ $is_sed = 1 ]
then
  if [ $uppercase = 1 ] || [ $lowercase = 1 ]
  then
    echo "Error: Not possible to use -l or -u option with the sed pattern."
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
          echo "Error: Not a directory."
          exit
      fi

      uppercase_recursive $name  # go to the function dedicated to recursively changing all files within the directory

  else  # not recursive

    if [ ! -f $name ]   # when not recursive, a file name must be given
    then
      echo "Error: Not a file."
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
          echo "Error: not a directory."
          exit
      fi

      lowercase_recursive $name  # go to the function dedicated to recursively changing all files within the directory

  else  # not recursive

    if [ ! -f $name ]   # when not recursive, a file name must be given
    then
      echo "Error: Not a file."
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
          echo "Error: not a directory."
          exit
        fi

      sed_recursive $name


    else    # not recursive

      if [ ! -f $name ]   # when not recursive, a file name must be given
      then
        echo "Error: Not a file."
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



