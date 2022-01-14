/*
 * Copyright (c) 2019 xfwcfw
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <string.h>

#include "keystore.h"
#include "kelf.h"

std::string getKeyStorePath()
{
	char *keydir;
#ifdef __linux__
	if (keydir = getenv("KEYDIR"))
	{
		return std::string(keydir) + "/PS2KEYS.dat";
	}
	else
	{
		printf("Set KEYDIR env variable.");
		exit(1);
	}
#else
	return std::string(getenv("USERPROFILE")) + "\\PS2KEYS.dat";
#endif
}

int decrypt(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("KEYDIR=<keydir> %s decrypt <input> <output>\n", argv[0]);
		return -1;
	}

	KeyStore ks;
	int ret = ks.Load(getKeyStorePath());
	if (ret != 0)
	{
		printf("Failed to load keystore: %d - %s\n", ret, KeyStore::getErrorString(ret).c_str());
		return ret;
	}

	Kelf kelf(ks);
	ret = kelf.LoadKelf(argv[1]);
	if (ret != 0)
	{
		printf("Failed to LoadKelf!\n");
		return ret;
	}
	ret = kelf.SaveContent(argv[2]);
	if (ret != 0)
	{
		printf("Failed to SaveContent!\n");
		return ret;
	}

	return 0;
}

int encrypt(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("%s decrypt <input> <output>\n", argv[0]);
		return -1;
	}

	KeyStore ks;
	int ret = ks.Load(getKeyStorePath());
	if (ret != 0)
	{
		printf("Failed to load keystore: %d - %s\n", ret, KeyStore::getErrorString(ret).c_str());
		return ret;
	}

	Kelf kelf(ks);
	ret = kelf.LoadContent(argv[1]);
	if (ret != 0)
	{
		printf("Failed to LoadContent!\n");
		return ret;
	}
	ret = kelf.SaveKelf(argv[2]);
	if (ret != 0)
	{
		printf("Failed to SaveKelf!\n");
		return ret;
	}

	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: %s <submodule> <args>\n", argv[0]);
		printf("Available submodules:\n");
		printf("\tdecrypt - decrypt and check signature of kelf files\n");
		printf("\tencrypt - encrypt and sign kelf files\n");
		return -1;
	}

	char *cmd = argv[1];
	argv[1] = argv[0];
	argc--;
	argv++;

	if (strcmp("decrypt", cmd) == 0)
		return decrypt(argc, argv);
	else if (strcmp("encrypt", cmd) == 0)
		return encrypt(argc, argv);

	printf("Unknown submodule!\n");
	return -1;
}
