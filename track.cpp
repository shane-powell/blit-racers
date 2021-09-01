#include "track.hpp"
#include "tiledata.hpp"

Track* LoadTrack(uint8_t trackId)
{
	switch (trackId)
	{
	case 0:
		return new Track(8, const_cast<uint8_t*>(map1), 128, 128, const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1),
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
			},
			blit::Size(24, 24), 
			{
				TileData(36,  false, true, false),
	TileData(37,  false, true, false),
	TileData(38,  false, true, false),
	TileData(39,  false, true, false),
	TileData(52,  false, true, false),
	TileData(53,  false, true, false),
	TileData(54,  false, true, false),
	TileData(55,  false, true, false),
	TileData(68,  false, true, false),
	TileData(69,  false, true, false),
	TileData(70,  false, true, false),
	TileData(71,  false, true, false),
	TileData(84,  false, true, false),
	TileData(85,  false, true, false),
	TileData(86,  false, true, false),
	TileData(87,  false, true, false)
			});
	case 1:
		return new Track(8, const_cast<uint8_t*>(map2), 128, 128, const_cast<uint8_t*>(tile_sheet_2), const_cast<uint8_t*>(tile_sheet_2), const_cast<uint8_t*>(tile_sheet_2),
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
			}, const_cast<uint8_t*>(map2_preview), "Bath tub", const_cast<uint8_t*>(boat1), {
				blit::Point(280, 448),
				blit::Point(288, 304),
				blit::Point(504, 320),
				blit::Point(656, 280),
				blit::Point(288, 304),
				blit::Point(728, 424),
				blit::Point(672, 536),
				blit::Point(536, 536),
				blit::Point(368, 544)
			}, blit::Size(16, 16), 
			{
				//Bubbles
				TileData(10,  false, true, false),
				TileData(11,  false, true, false),
				TileData(12,  false, true, false),
				TileData(13,  false, true, false),
				TileData(14,  false, true, false),
				TileData(15,  false, true, false),
				TileData(27,  false, true, false),
				TileData(28,  false, true, false),
				TileData(29,  false, true, false),
				TileData(30,  false, true, false),
				TileData(31,  false, true, false),
				TileData(32,  false, true, false),

				//Duck
				TileData(32,  false, true, false),
				TileData(33,  false, true, false),
				TileData(34,  false, true, false),
				TileData(35,  false, true, false),
				TileData(36,  false, true, false),

				TileData(48,  false, true, false),
				TileData(49,  false, true, false),
				TileData(50,  false, true, false),
				TileData(51,  false, true, false),
				TileData(52,  false, true, false),

				TileData(64,  false, true, false),
				TileData(65,  false, true, false),
				TileData(66,  false, true, false),
				TileData(67,  false, true, false),
				TileData(68,  false, true, false),

				TileData(80,  false, true, false),
				TileData(81,  false, true, false),
				TileData(82,  false, true, false),
				TileData(83,  false, true, false),
				TileData(84,  false, true, false),

				TileData(96,  false, true, false),
				TileData(97,  false, true, false),
				TileData(98,  false, true, false),
				TileData(99,  false, true, false),
				TileData(100,  false, true, false)
			});
		break;
	}

}