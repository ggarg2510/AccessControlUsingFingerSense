
#include <pic.h>
#include "lcd.h"
#include "keypd.h"
#include "FingerPrintSensor_R307.h"
#include "eeprom.h"

__CONFIG(0x3F3A);
#define RELAY	RB1

unsigned char EraseAllUsersAfterAdminConfirm()
{
	unsigned char isEraseAllUsers=1;
	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		isEraseAllUsers = 0;
		//Match previous admin user
		unsigned char UserNo;
		unsigned char result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && UserNo == 0)
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
				isEraseAllUsers = 1;
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
	if(isEraseAllUsers == 1)
	{
		EraseAllUsers();
		ee_eraseNPos(MAX_R307_USERS);
		ee_write(TOT_USERS_COUNT_ADD,0x00);

		LcdClear();
		LcdPrint("All Users\n Deleted",0,0);
		Delay(2);
	}	
	return 0;
}	

unsigned char ControlAccess()
{
	unsigned char UserNo=0;
	unsigned char result=0;
	RB5=0;

	while(1)
	{
		LcdClear();
		LcdPrint("Place Finger", 0,0);
		result=HiSpeedScan0(&UserNo);
				
		if(result == 0xFF) break;
		else if(result == 0x01)
		{
			unsigned char buff[10];
			memset1(buff, 0, sizeof(buff));
			
			LcdClear();
			LcdPrint("User Valid\nUser No:", 0,0);
			DecToStr(UserNo, buff, 2);
			LcdPrint(buff, 1, 9);
			RELAY = 1; 
//			RB5=1;
//			buzzOffCounter=1000;
			Delay(3);
			RELAY = 0; 
			break;
		}	
		else if(result == 0x00)
		{
			LcdClear();
			LcdPrint("Invalid User", 0,0);
			RB5=1;
			Delay(2);
//			LcdDelay(200);
			RB5=0;
			break;
		}	
	}	
	return 0;	
}	


unsigned char AlterAdminUser()
{
	unsigned char Create0thPositionUser=1;

	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		Create0thPositionUser = 0;
		//Match previous admin user
		unsigned char UserNo;
		unsigned char result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && UserNo == 0)
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
				if(DeleteUser(0x00) == 1)
				{
					LcdClear();
					LcdPrint("Admin Deleted",0,0);
					Delay(1);
					Create0thPositionUser = 1;
				}
				else
				{
					LcdClear();
					LcdPrint("Can't Delete\n Admin",0,0);
					Delay(2);
					return 0;
				}
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
	if(Create0thPositionUser == 1)
	{
		LcdClear();
		LcdPrint("Plz Register\n new Admin",0,0);
		Delay(2);
		LcdClear();
		LcdPrint("Place Finger",0,0);
		Delay(2);
		unsigned char NewUserResult = AddNewUser(0x00);
		if(NewUserResult == 1)
		{
			LcdClear();
			LcdPrint("Admin \n  Registered",0,0);
			Delay(2);
		}
		else if(NewUserResult == 0xFF)	return 0;
		else
		{
			LcdClear();
			LcdPrint("Error In\n  Registeration",0,0);
			Delay(2);
		}		
	//	addUser(0);
	}	
return 0;	
}	


unsigned char AddNewUserAfterAdminConfirm()
{
	unsigned char isAddNewUser=1;
	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		isAddNewUser = 0;
		//Match previous admin user
		unsigned char UserNo;
		unsigned char result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && UserNo == 0)
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
				isAddNewUser = 1;
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
	if(isAddNewUser == 1)
	{
		signed char _availSpaceResult = getNextAvailLocation();
		if(_availSpaceResult == -1)	
		{
			LcdClear();
			LcdPrint("Not Enough\n Memory",0,0);
			Delay(2);
		}	
		else
		{
			LcdClear();
			LcdPrint("New User\nPlace Finger",0,0);
			unsigned char availSpaceResult = _availSpaceResult;
			unsigned char AddNewUserResult = AddNewUser(availSpaceResult);
			if(AddNewUserResult == 1)
			{
				unsigned char buff[10];
				LcdClear();
				LcdPrint("New User\n Created @:",0,0);
				DecToStr(availSpaceResult, buff, 2);
				LcdPrint(buff, 1, 12);
				Delay(2);
			}	
			else if(AddNewUserResult == 0xFF)	return 0;
			else
			{
				LcdClear();
				LcdPrint("Error In\n User Creation",0,0);
				Delay(2);
			}	
		}
	}	
	return 0;
}	


unsigned char DeleteUserAfterAdminConfirm()
{
	unsigned char isDeleteUser=1;

	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		isDeleteUser = 0;
		//Match previous admin user
		unsigned char UserNo;
		unsigned char result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && UserNo == 0)
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
		
				unsigned char DelUserNo = 1;
				unsigned char buff[5];
				LcdClear();
				LcdPrint("Select User",0,0);
				DecToStr(DelUserNo, buff, 2);
				LcdPrint(buff, 1, 5);
				while(1)
				{
					KeyScan();
					if(IsKeyPending())
					{	
						unsigned char key = GetKey();
						if(key == '1'){ if(DelUserNo<MAX_R307_USERS) DelUserNo++; }
						else if(key == '2') {if(DelUserNo > 0) DelUserNo--; }
						else if(key == '3') return 0;
						else if(key == '4') break;

						DecToStr(DelUserNo, buff, 2);
						LcdPrint(buff, 1, 5);
					}
				}
				
				if(DeleteUser(DelUserNo) == 1)
				{
					unsigned char buff[10];
					LcdClear();
					LcdPrint("User Deleted\nUser No:",0,0);
					DecToStr(DelUserNo, buff, 2);
					LcdPrint(buff, 1, 9);
					releaseEElocation(DelUserNo);
					Delay(2);
				}	
				else
				{
					LcdClear();
					LcdPrint("Cannot Delete",0,0);
					Delay(2);
				}	
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
return 0;
}	

unsigned char menu(const unsigned char *s)
{
	unsigned char keyHit=0xFF;
	LcdClear();
	LcdPrint(s, 0,0);
	while(1)
	{
		KeyScan();
		if(IsKeyPending())
		{
			keyHit = GetKey();
			if(keyHit == '1' || keyHit == '2' || keyHit == '3')
				return keyHit;
		}		
	}
	return 0;
}
void interrupt isr()
{
	if(RCIF == 1)
	{
		PutUartByte(RCREG);
		RCIF = 0;		
	}
}
void main()
{
	unsigned char menuResponse=0xFF;
	
	TRISC=0X8F;
	LcdInit();
	RELAY = 0; 
	LcdPrint("Welcome", 0,0);
	Delay(1);
	UartInit();
	
	VerifyPswd();
//	ChkFinger(0);
//	while(1)
//	ScanNthFinger(0);
	
	while(1)
	{
		menuResponse = menu("1 For Access\n2 For Alter User");
		if(menuResponse == '1')	{ ControlAccess();}
		else if(menuResponse == '2')
		{
			while(1)
			{
				menuResponse = menu("1 Change Admin\n2 Add/Del User");
				if(menuResponse == '1') AlterAdminUser();	
				else if(menuResponse == '2')
				{
					while(1)
					{
						menuResponse = menu("1 Add New User\n2 Delete Old User");
						if(menuResponse == '1'){AddNewUserAfterAdminConfirm();} 
						else if(menuResponse == '2')
						{
							while(1)
							{
								menuResponse = menu("1 Delete All\n2 Delete Single");
								if(menuResponse == '1'){EraseAllUsersAfterAdminConfirm();}
								else if(menuResponse == '2'){DeleteUserAfterAdminConfirm();}
								else if(menuResponse == '3')break;
							}	
						}	
						else if(menuResponse == '3')break;
					}
				}
				else if(menuResponse == '3')break;
			}	
		}
	}
}

