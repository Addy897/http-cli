#pragma once
#include <bits/stdc++.h>
#include <ctime>
#include <iostream>

typedef std::map<std::string, std::string> HEADERS;
typedef unsigned char Byte;
typedef std::vector<Byte> BytesArray;
typedef std::pair<time_t, std::string> TimedContent;
typedef std::map<std::string, std::unique_ptr<TimedContent>> Cache;
