#include <iostream>
#include <unistd.h>
#include "Banner.hpp"

void printFrame (const char *chars[][6], int count) {
	int row;
	int col;

	row = 0;
	while (row < 6) {
		col = 0;
		while (col < count) {
			std::cout << chars[col][row] << "  ";
			col++;
		}
		std::cout << std::endl;
		row++;
	}
}

// clang-format off
void printBannerTypewriter () {
	const char *F[6] = {"███████╗",
						"██╔════╝",
						"█████╗  ",
						"██╔══╝  ",
						"██║     ",
						"╚═╝     "};

	const char *T[6] = {"████████╗",
						"╚══██╔══╝",
						"   ██║   ",
						"   ██║   ",
						"   ██║   ",
						"   ╚═╝   "};

	const char *U[6] = {"        ",
						"        ",
		 				"        ",
						"        ",
						"███████╗",
						"╚══════╝"};

	const char *I[6] = {"██╗",
						"██║",
						"██║",
						"██║",
						"██║",
						"╚═╝"};

	const char *R[6] = {"██████╗ ",
						"██╔══██╗",
						"██████╔╝",
						"██╔══██╗",
						"██║  ██║",
						"╚═╝  ╚═╝"};

	const char *C[6] = {" ██████╗",
						"██╔════╝",
						"██║     ",
						"██║     ",
						"╚██████╗",
						" ╚═════╝"};
	const char *chars[6][6] = {
		{F[0], F[1], F[2], F[3], F[4], F[5]},
		{T[0], T[1], T[2], T[3], T[4], T[5]},
		{U[0], U[1], U[2], U[3], U[4], U[5]},
		{I[0], I[1], I[2], I[3], I[4], I[5]},
		{R[0], R[1], R[2], R[3], R[4], R[5]},
		{C[0], C[1], C[2], C[3], C[4], C[5]}};
	// clang-format on
	int frame;

	frame = 1;
	while (frame <= 6) {
		std::cout << "\033[H\033[J";
		printFrame (chars, frame);
		// usleep (250000);
		for (int i = 0; i < 5e8; ++i) {
		}
		frame++;
	}
}

// " ███████╗ ████████╗          ██╗ ██████╗   ██████╗",
// " ██╔════╝ ╚══██╔══╝          ██║ ██╔══██╗ ██╔════╝",
// " █████╗      ██║             ██║ ██████╔╝ ██║     ",
// " ██╔══╝      ██║             ██║ ██╔══██╗ ██║     ",
// " ██║         ██║    ███████╗ ██║ ██║  ██║ ╚██████╗",
// " ╚═╝         ╚═╝    ╚══════╝ ╚═╝ ╚═╝  ╚═╝  ╚═════╝"
