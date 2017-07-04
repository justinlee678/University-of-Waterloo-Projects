#include "EV3_FileIO.c"

const int riptideSize=31;

struct riptide
{
char chord[riptideSize];//Array of Chords
int strummingPattern[riptideSize];//two strumming patterns possible, so either 0 and 1
};

void riptideStrumPattern (int pattern,int & strummingEncoder)
{
	//There are 3 different strumming patterns
	//Pattern 0 and 1 are similar, but pattern 1 is x2 the pattern 0
	if (pattern==0||pattern==1)
	{
		for (int x=0;x<=pattern;x++)
		{
			strummingEncoder+=180;
			setMotorSpeed(motorD,70);
			while (getMotorEncoder(motorD)<strummingEncoder)
			{}
			setMotorSpeed(motorD,0);
			delay(200):

			strummingEncoder+=180;
			setMotorSpeed(motorD,70);
			while (getMotorEncoder(motorD)<strummingEncoder)
			{}
			setMotorSpeed(motorD,0);
			delay(400);

			strummingEncoder+=540;
			setMotorSpeed(motorD,90);
			while(getMotorEncoder(motorD)<strummingEncoder)
			{}
			setMotorSpeed(motorD,0);
		}
	}
	else //strum and pause for 2 secs
	{
		strummingEncoder+=180;
		setMotorSpeed(motorD,100);
		while(getMotorEncoder(motorD)<strummingEncoder)
		{}
		setMotorSpeed(motorD,0);
		wait1Msec(2000);
	}

}
////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
void beginProgram()
{
	SensorType[S1]=sensorEV3_Touch;
	//greet the user and give song options
	displayBigTextLine(0,"Welcome to");
	displayBigTextLine(3,"GuitarBot");
	displayBigTextLine(9,"Press to ");
	displayBigTextLine(11,"continue");

	while (SensorValue(S1)==0)//waits for press
	{}
	while (SensorValue(S1)==1)//wait for release
	{}
	eraseDisplay();

}
void jukeBox(string & songColour)
{
	//loop until a new colour (from the options) is detected
	SensorType[S2]=sensorEV3_Color;
	SensorType[S1]=sensorEV3_Touch;

	songColour="No Colour";

	displayBigTextLine(0,"Riptide - RED");
	displayBigTextLine(2,"MTE Song - BLUE");
	displayBIgTextLine(8,"Insert card");

	delay(1000);
	do
	{
		if (getColorName(S2)==colorRed)
		{
			songColour="Red";
		}
		else if (getColorName(S2)==colorBlack)
		{
			songColour="Blue";
		}

	}while (songColour=="No Colour");

	eraseDisplay();
	displayBigTextLine(5,"\tPRESS TO START");
	while (SensorValue(S1)==0)//waits for press
	{}
	while (SensorValue(S1)==1)//wait for release
	{}
	eraseDisplay();
}

void loadSong(riptide riptideSong)
{
	TFileHandle chords;
	TFileHandle patternFile;
	bool fileOkayChords = openReadPC(chords,"riptide.txt");
	bool fileOkayPatternFile = openReadPC(patternFile,"strumming.txt");
	char chord;
	for (int x=0;x<riptideSize;x++)
	{
	readCharPC(chords,chord);
	riptideSong.chord[x]=	chord;
	}
	int pattern;
	for (int x=0;x<riptideSize;x++)
	{
	readIntPC(patternFile,pattern);
	riptideSong.strummingPattern[x]=pattern;
	}


}

///////////////////////////////Chord Functions/////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void moveUpOrDownChord(bool up,int & upDownEncoder)
{
	if (up)
	{
		setMotorSpeed(motorA, 100);
		setMotorSpeed(motorB, -100);
		upDownEncoder+=100;
		while (getMotorEncoder(motorA)<upDownEncoder)
		{}
		setMotorSpeed(motorA, 0);
		setMotorSpeed(motorB, 0);
	}
	else
	{
		setMotorSpeed(motorA, -100);
		setMotorSpeed(motorB, 100);
		upDownEncoder-=100;
		while (getMotorEncoder(motorA)>upDownEncoder)
		{}
		setMotorSpeed(motorA, 0);
		setMotorSpeed(motorB, 0);
	}
}

void turnToRightChord(int chordPosition, int previousPosition, int & spinChordEncoder,int & upDownEncoder)
{
//if the desiredChord is equal to previous Chord, don't lift up
	int initialEncoder=spinChordEncoder;

	if (chordPosition!=previousPosition)
	{
		moveUpOrDownChord(true,upDownEncoder);
	//rotate the chord depending on the last chord
		if (chordPosition-previousPosition>0)
		{
			if (chordPosition-previousPosition==3)
				spinChordEncoder+=90;
			else
				spinChordEncoder-=(chordPosition-previousPosition)*90;
		}
		else//chordPosition-previousPosition<0
		{

			if (chordPosition-previousPosition==-3)
			{
				spinChordEncoder-=90;
			}
			else
				spinChordEncoder-=(chordPosition-previousPosition)*90;
		}

		//SPIN THE CHORD
		if (spinChordEncoder>initialEncoder)//encoder was added
		{
			setMotorSpeed(motorC,20);
			while (getMotorEncoder(motorC)<spinChordEncoder)
			{}
			setMotorSpeed(motorC,0);
		}
		else
		{
			setMotorSpeed(motorC,-20);
			while (getMotorEncoder(motorC)>spinChordEncoder)
			{}
			setMotorSpeed(motorC,0);
		}
		moveUpOrDownChord(false,upDownEncoder);
	}
}

int playAm(int previousPosition,int & spinChordEncoder,int & upDownEncoder)
{
int const AmPosition=1;
turnToRightChord(AmPosition, previousPosition,spinChordEncoder,upDownEncoder);
return 1;
}
////////////////////
int playG (int previousPosition, int & spinChordEncoder,int & upDownEncoder)
{
int const GPosition=2;
turnToRightChord(GPosition, previousPosition,spinChordEncoder,upDownEncoder);
return 2;
}
////////////////////
int playC(int previousPosition,int & spinChordEncoder, int & upDownEncoder)
{
int const CPosition=4;
turnToRightChord(CPosition,previousPosition,spinChordEncoder,upDownEncoder);
return 4;
}
////////////////////
int playD(int previousPosition,int & spinChordEncoder, int & upDownEncoder)
{
int const DPosition=3;
turnToRightChord(DPosition,previousPosition,spinChordEncoder,upDownEncoder);
return 3;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void playSong(riptide RiptideSong)
{
	resetMotorEncoder(motorA);
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	resetMotorEncoder(motorD);
	int upDownEncoder=0;
	int strummingEncoder=0;
	int spinChordEncoder=0;
	bool emergencyExit=false;//for the
	//LOWER THE PRISM
	moveUpOrDownChord(false,upDownEncoder);//moves down
	int previousChord=1;//chord starts at Aminor
	int NextChord;

	for (int x=0;x<riptideSize&&!emergencyExit;x++)
	{
		//Switch the Chord
		NextChord=RiptideSong.chord[x];
		if (NextChord=='A')
		{
			previousChord=playAm(previousChord,spinChordEncoder, upDownEncoder);
		}
		else if (NextChord=='G')
		{
			previousChord=playG(previousChord,spinChordEncoder, upDownEncoder);
		}
		else if (NextChord=='C')
		{
			previousChord=playC(previousChord,spinChordEncoder, upDownEncoder);
		}
		else//Dmajor
		{
			previousChord=playD(previousChord,spinChordEncoder, upDownEncoder);
		}

		riptideStrumPattern(RiptideSong.strummingPattern[x],strummingEncoder);
	}
}
////////////////////
task main()
{

	struct riptide riptideSong;
	//int upDownEncoder=0;
	string songColour;

	beginProgram();
	jukeBox(songColour);
	if (songColour=="Red")//riptide
	{
		displayBigTextLine (4,"PLAYING RIPTIDE");
	loadSong(riptideSong);
		playSong(riptideSong);
	}
	else if (songColour=="Blue")
	{
		displayBigTextLine(5, "Song Under");
		displayBigTextLine(7, "Development");
		displayBigTextLine(9, " :'( ");
		delay(3000);
	}
	delay(1000);
//moveUpOrDownChord(false,upDownEncoder);
}
