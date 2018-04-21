#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <locale.h>
#include <time.h>
#include <errno.h>

#include <seclib.h>
#include "project.h"


// set promisc mode on the specific NIC
void nicsetpromisc(UINT num, char **arg)
{
	if (num < 1)
	{
		Print("Usage: nicsetpromisc <NICNAME>\n");
	}
	else
	{
		char *nicname = arg[0];
		ETH *e;

		InitEth();

		e = OpenEth(nicname, false, false, NULL);
		if (e == NULL)
		{
			Print("OpenEth Error.\n");

			FreeEth();
		}
		else
		{
			UCHAR *buf;
			Print("Started.\n");
			EthGetPacket(e, &buf);
			SleepThread(INFINITE);
		}
	}
}

// list the NICs on the system
void niclist(UINT num, char **arg)
{
	TOKEN_LIST *t;
	UINT i;

	InitEth();

	t = GetEthList();

	Print("----------\n");
	for (i = 0;i < t->NumTokens;i++)
	{
		Print("%s\n", t->Token[i]);
	}
	Print("----------\n");

	FreeToken(t);

	FreeEth();
}


// Test function definition list
void test(UINT num, char **arg)
{
	if (true)
	{
		Print("Test! %u\n", IsX64());

		Temp_TestFunction("Nekosan");
		return;
	}
}

typedef void (TEST_PROC)(UINT num, char **arg);

typedef struct TEST_LIST
{
	char *command_str;
	TEST_PROC *proc;
} TEST_LIST;

TEST_LIST test_list[] =
{
	{ "test", test },
	{ "niclist", niclist },
	{ "nicsetpromisc", nicsetpromisc },
};

// Test function
void TestMain(char *cmd)
{
	char tmp[MAX_SIZE];
	bool first = true;
	bool exit_now = false;

	Print("Test Program\n");

#ifdef	OS_WIN32
	MsSetEnableMinidump(false);
#endif	// OS_WIN32
	while (true)
	{
		Print("TEST>");
		if (first && StrLen(cmd) != 0 && g_memcheck == false)
		{
			first = false;
			StrCpy(tmp, sizeof(tmp), cmd);
			exit_now = true;
			Print("%s\n", cmd);
		}
		else
		{
			GetLine(tmp, sizeof(tmp));
		}
		Trim(tmp);
		if (StrLen(tmp) != 0)
		{
			UINT i, num;
			bool b = false;
			TOKEN_LIST *token = ParseCmdLine(tmp);
			char *cmd = token->Token[0];
			if (!StrCmpi(cmd, "exit") || !StrCmpi(cmd, "quit") || !StrCmpi(cmd, "q"))
			{
				FreeToken(token);
				break;
			}
			else if (StrCmpi(cmd, "?") == 0 || StrCmpi(cmd, "help") == 0)
			{
				num = sizeof(test_list) / sizeof(TEST_LIST);
				Print("Available %u commands:\n", num);
				for (i = 0;i < num;i++)
				{
					Print(" %s\n", test_list[i].command_str);
				}
				Print("\n");
			}
			else
			{
				num = sizeof(test_list) / sizeof(TEST_LIST);
				for (i = 0;i < num;i++)
				{
					if (!StrCmpi(test_list[i].command_str, cmd))
					{
						char **arg = Malloc(sizeof(char *) * (token->NumTokens - 1));
						UINT j;
						for (j = 0;j < token->NumTokens - 1;j++)
						{
							arg[j] = CopyStr(token->Token[j + 1]);
						}
						test_list[i].proc(token->NumTokens - 1, arg);
						for (j = 0;j < token->NumTokens - 1;j++)
						{
							Free(arg[j]);
						}
						Free(arg);
						b = true;
						Print("\n");
						break;
					}
				}
				if (b == false)
				{
					Print("Invalid Command: %s\n\n", cmd);
				}
			}
			FreeToken(token);

			if (exit_now)
			{
				break;
			}
		}
	}
	Print("Exiting...\n\n");
}

// Entry point
int main(int argc, char *argv[])
{
	bool memchk = false;
	char cmd[MAX_SIZE];

	cmd[0] = 0;

	SetHamMode();

	InitMayaqua(memchk, true, argc, argv);
	EnableProbe(false);
	InitCedar();
	SetHamMode();

	TestMain(cmdline);

	FreeCedar();
	FreeMayaqua();

	return 0;
}

