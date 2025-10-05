
.pragma library


function handleKeys(event, view) {
    let delegateHeight = view.contentItem.children[0]?.height || 1;
    let delegateWidth = view.contentItem.children[0]?.width || 1;

    let visibleRows = Math.floor(view.height / delegateHeight);
    let visibleColumns = Math.floor(view.width / delegateWidth)
    let itemsPerPage = visibleRows * visibleColumns;

    if (view.keyNavigationEnabled == false)
        return

    event.accepted = true;
    switch (event.key) {
        case Qt.Key_PageDown:
            view.currentIndex = view.currentIndex + itemsPerPage;
            break;
        case Qt.Key_PageUp:
            view.currentIndex = view.currentIndex - itemsPerPage;
            break;
        case Qt.Key_Home:
            view.currentIndex = 0;
            break;
        case Qt.Key_End:
            view.currentIndex = view.count - 1;
            break;
        default:
            event.accepted = false;
    }
}
