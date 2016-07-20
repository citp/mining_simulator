//
//  simulationState.h
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef logging_h
#define logging_h



#define _ERROROUTPUT 1
//#define _GAMEINFO 1
//#define _COMMENTARY 1

#ifdef _GAMEINFO
#define GAMEINFO(arg) std::cout << arg
#define GAMEINFOBLOCK(arg) arg
#else
#define GAMEINFO(arg)
#define GAMEINFOBLOCK(arg)
#endif

#ifdef _COMMENTARY
#define COMMENTARY_ON true
#define COMMENTARY(arg) std::cout << arg
#define COMMENTARYBLOCK(arg) arg
#else
#define COMMENTARY_ON false
#define COMMENTARY(arg)
#define COMMENTARYBLOCK(arg)
#endif

#ifdef _ERROROUTPUT
#define ERROR(arg) std::cout << arg
#else
#define ERROR(arg)
#endif


#endif /* logging_h */
