#include "track.hpp"

Track* LoadTrack(uint8_t trackId)
{
	switch (trackId)
	{
	case 0:
		return new Track(8, const_cast<uint8_t*>(map1), const_cast<uint8_t*>(spritesheet), 128, 128, const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1),
			{
				Position(blit::Point(270,480),180),
				Position(blit::Point(305,480),180),
				Position(blit::Point(270,520),180),
				Position(blit::Point(310,520),180)
			}
			,
			{
				blit::Point(270, 440),
				blit::Point(270, 412),
				blit::Point(270, 346),
				blit::Point(286, 297),
				blit::Point(331,288),
				blit::Point(405, 287),
				blit::Point(486, 305),
				blit::Point(555, 334),
				blit::Point(636, 300),
				blit::Point(699,283),
				blit::Point(726, 319),
				blit::Point(720, 394),
				blit::Point(728, 477),
				blit::Point(653,526),
				blit::Point(589, 550),
				blit::Point(498, 557),
				blit::Point(399, 548),
				blit::Point(302, 536),
				blit::Point(257,501),
				blit::Point(262, 469)
			}, const_cast<uint8_t*>(map1_preview), "Kitchen", const_cast<uint8_t*>(car1), {
				blit::Point(280, 448),
				blit::Point(288, 304),
				blit::Point(504, 320),
				blit::Point(656, 280),
				blit::Point(288, 304),
				blit::Point(728, 424),
				blit::Point(672, 536),
				blit::Point(536, 536),
				blit::Point(368, 544)
			});
	case 1:
		return new Track(8, const_cast<uint8_t*>(map2), const_cast<uint8_t*>(spritesheet), 128, 128, const_cast<uint8_t*>(tile_sheet_2), const_cast<uint8_t*>(tile_sheet_2), const_cast<uint8_t*>(tile_sheet_2),
			{
				Position(blit::Point(270,480),180),
				Position(blit::Point(305,480),180),
				Position(blit::Point(270,520),180),
				Position(blit::Point(310,520),180)
			}
			,
			{
				blit::Point(270, 440),
				blit::Point(270, 412),
				blit::Point(270, 346),
				blit::Point(286, 297),
				blit::Point(331,288),
				blit::Point(405, 287),
				blit::Point(486, 305),
				blit::Point(555, 334),
				blit::Point(636, 300),
				blit::Point(699,283),
				blit::Point(726, 319),
				blit::Point(720, 394),
				blit::Point(728, 477),
				blit::Point(653,526),
				blit::Point(589, 550),
				blit::Point(498, 557),
				blit::Point(399, 548),
				blit::Point(302, 536),
				blit::Point(257,501),
				blit::Point(262, 469)
			}, const_cast<uint8_t*>(map1_preview), "Kitchen", const_cast<uint8_t*>(boat1), {
				blit::Point(280, 448),
				blit::Point(288, 304),
				blit::Point(504, 320),
				blit::Point(656, 280),
				blit::Point(288, 304),
				blit::Point(728, 424),
				blit::Point(672, 536),
				blit::Point(536, 536),
				blit::Point(368, 544)
			});
		break;
	}

}