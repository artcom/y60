#!/bin/bash -e

if [ "$1" == "-h" -o "$1" == "--help" -o "$1" == "" ]; then
    echo "usage: make_project.sh Projectname [CHECKOUT_PATH, default is '../../]"
    echo "Creates the default project directories and start scripts."
    exit 1
    
fi
CHECKOUT_PATH="../../.."

if [ "$2" != "" ]; then
    CHECKOUT_PATH=$2
fi

echo "relative y60 path from new project directory: '"$CHECKOUT_PATH"'"
 
PROJECT_NAME=$1

if [ -d "$PROJECT_NAME" ]; then
    echo "### ERROR: Project directory '$PROJECT_NAME' already exists"
    exit 1
fi

echo "------------------------------------------------------------"
echo "  Creating new project: $PROJECT_NAME"
echo "------------------------------------------------------------"

####################################################################
echo "  - Creating directory structure"
####################################################################

mkdir $PROJECT_NAME
cd $PROJECT_NAME
mkdir SCRIPTS
mkdir CONFIG
mkdir TESTS
mkdir TEX
mkdir FONTS
mkdir MODELS
mkdir SOUNDS
mkdir MOVIES
mkdir LAYOUT

echo "  - Creating skeleton scripts file"

####################################################################
echo "      - $PROJECT_NAME.js"
####################################################################

# Find Skeleton.js
SKELETON="$CHECKOUT_PATH/y60/js/spark/SparkSkeleton.js"
if [ ! -e "$SKELETON" ]; then
    echo "### ERROR: Could not find SparkSkeleton.js in '$CHECKOUT_PATH/y60/js/spark/SparkSkeleton.js'"
    exit 1;
fi

# Replace the word Skeleton with the project name
sed s/Skeleton/$PROJECT_NAME/g "$SKELETON" > SCRIPTS/$PROJECT_NAME.js

####################################################################
echo "      - $PROJECT_NAME.sh"
####################################################################

START_SCRIPT=${PROJECT_NAME}.sh
echo -e "#!/bin/bash\ny60 -I \"SCRIPTS;.\" $PROJECT_NAME.js rehearsal \$*" > $START_SCRIPT
chmod a+rx $START_SCRIPT

####################################################################
echo "      - $PROJECT_NAME.spark"
####################################################################

# Find Skeleton.spark
SKELETON="$CHECKOUT_PATH/y60/js/spark/Skeleton.spark"
if [ ! -e "$SKELETON" ]; then
    echo "### ERROR: Could not find Skeleton.spark"
    exit 1;
fi

# Replace the word Skeleton with the project name
sed s/Skeleton/$PROJECT_NAME/g "$SKELETON" > LAYOUT/$PROJECT_NAME.spark

####################################################################
#DEFAULT_FONT=arial
#echo "      - default font: "$DEFAULT_FONT
####################################################################

#cp $CHECKOUT_PATH/testmodels/fonts/$DEFAULT_FONT.ttf FONTS/$DEFAULT_FONT-normal.ttf

####################################################################
echo "      - watchdog.xml"
####################################################################

cat << __EOF > watchdog.xml
<WatchdogConfig logfile="LOGFILES\watch.log" watchFrequency="15">
    <Application binary="y60.exe">
        <EnvironmentVariables>
            <EnvironmentVariable name="AC_LOG_CONSOLE_FORMAT">
                <![CDATA[FULL]]>
            </EnvironmentVariable>
            <EnvironmentVariable name="AC_LOG_VERBOSITY">
                <![CDATA[INFO]]>
            </EnvironmentVariable>
        </EnvironmentVariables>
        <Arguments>
            <Argument>-I "SCRIPTS"</Argument>
            <Argument>--std-logfile LOGFILES\out_%h_%d.log</Argument>
            <Argument>$PROJECT_NAME.js</Argument>
        </Arguments>
        <WaitDuringStartup>0</WaitDuringStartup>
        <WaitDuringRestart>5</WaitDuringRestart>
        <!--<Memory_Threshold>50000</Memory_Threshold>-->
        <!--<RestartTime>5:00</RestartTime>-->
        <!--<CheckMemoryTime>04:00</CheckMemoryTime>-->
        <!--CheckTimedMemoryThreshold>150000</CheckTimedMemoryThreshold>-->
    </Application>
</WatchdogConfig>
__EOF
