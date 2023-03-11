#include "../tracking/Tracking.hpp"

$execute {
    new EventListener<EditorFilter>(+[](EditorEvent* ev) {
        log::info("{}", ev->toDiffString());
    });
}
