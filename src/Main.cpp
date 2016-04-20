/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Created by: Matthieu Pinard, Ecole des Mines de Saint-Etienne, matthieu.pinard@etu.emse.fr
15-04-2016: Initial release
*/

#include "ThreadManager.h"
#include <thread> 
#include <chrono> 
#include <iostream>


// Callback function. Here we try to update when the Global Value exceeds these prime numbers:
const size_t Primes[] = {
	757, 1783, 3833, 7937,
	16141, 32537, 65327, 130873,
	261977, 524123, 1048433, 2097013,
	4194167, 8388473, 16777121, 33554341,
	67108777, 134217649, 268435399, 536870869,
	1073741789, 2147483629, 4294967291
};
static size_t PrimeIdx = 0U;

static ThreadManager Manager([&](uInt GlobalValue) -> uInt {
	if (GlobalValue > Primes[PrimeIdx]) {
		++PrimeIdx;
	}
	return Primes[PrimeIdx];
});

static thread_local ThreadValue Value(Manager);

int main() {
	auto thrCount = 2U;
	auto itCount = 300000000U;
	for (int k = 0; k < 50; ++k) {
		Manager.DTOR_ThreadValuesSum = 0;
		PrimeIdx = 0U;
		auto Begin = std::chrono::high_resolution_clock::now();
		std::vector<std::thread> Threads(thrCount);
		for (auto i = 0U; i < thrCount; i++) {
			Threads[i] = std::thread([&]() {
				for (auto j = 0U; j < itCount; j++) {
					Value.Increment();
				}
			});
		}
		for (auto i = 0U; i < thrCount; i++) {
			Threads[i].join();
		}
		auto End = std::chrono::high_resolution_clock::now();
		std::cout << "Time elapsed: " << (End - Begin).count() / 1e9 << "\t\tGlobal Value: " << Manager.GetGlobalValue() << "\n";
	}
	getchar();
	return 0;
}