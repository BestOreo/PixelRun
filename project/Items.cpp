#include "Items.h"
#include "GameManager.h"

using namespace std;
using namespace glm;
void Item::DestroyItem() {
	obj->SetActive(false);
}