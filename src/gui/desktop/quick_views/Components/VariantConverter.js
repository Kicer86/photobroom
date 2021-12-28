
function variantToString(type, variant) {

    if (type === TagEnums.Date) {
        var date = new Date(variant);
        return date.toLocaleDateString(Qt.locale(), Locale.ShortFormat);
    } else if (type == TagEnums.Time) {
        var date = new Date(variant);
        return date.toLocaleTimeString(Qt.locale(), "hh:mm:ss");
    } else {
        return variant === undefined? null : variant.toString();
    }
}
