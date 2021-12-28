
function variantToString(type, variant) {

    if (type === TagEnums.Date) {
        var date = new Date(variant);
        return date.toLocaleDateString(Qt.locale(), Locale.ShortFormat);
    }
    else
        return variant;
}
