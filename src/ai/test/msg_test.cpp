#include <iostream.h>
#include <conio.h>
#include "ai/msg.h"
#include "ai/time.h"

using namespace reaper::ai;

void main()
{
	unsigned Player1 = 1;
	unsigned Player2 = 2;

	Time timer;
	MsgRouter router(10);

	Message tmp_msg;
	
	timer.reset();
	cout << "Time\n";
	while(timer.now() <= 20.0){
		cout << timer.now() << "   ";
		
		while(router.msg_waiting(Player2)){
			tmp_msg = router.receive(Player2);
			cout << "Msg received: " << tmp_msg.name << "  ";
		}	
		cout << "\n";	
		
		if(timer.now() == 5.0){
			Message m1(MSG_DEATH, Player1, Player2, (timer.now()+3.0));
			router.send(m1);
		}
		if(timer.now() == 14.0){
			Message m2(MSG_DAMAGE, Player1, Player2, (timer.now()+3.0), 90);
			router.send(m2);
		}
		
		timer.update();
		router.update(timer.now());
	}

	if(!router.msg_waiting(Player2))
		cout << "Queue empty";

}

