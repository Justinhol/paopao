#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;

class MyScene : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	cocos2d::TMXLayer* mask;
	cocos2d::TMXTiledMap* map;

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(MyScene);

	CocosDenshion::SimpleAudioEngine* myMusicer;
	cocos2d::EventListenerKeyboard* listener;
	cocos2d::PhysicsBody *playerbody;

	void toch_Move(float dt);
	void touchMove(cocos2d::Ref* pSender, Widget::TouchEventType event);
	bool IsCanReach(float x, float y);
	bool IsCanPass(float x, float y);
	void LabelIn(cocos2d::Node* label);
	void RoundOne(float);
	void Ready(float);
	void Start(float);
	void move(float dt, cocos2d::EventKeyboard::KeyCode keycode);
	void update(float dt);
	void ExplodeRemove();
	void ShowMenu();
	void ShowMenu1();
	void progress(Ref* pSender);
	void progress1(Ref* pSender);
	void resume(float);
	void next(float);
	void exit(Ref* pSender);
};

