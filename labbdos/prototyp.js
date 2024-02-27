Array.prototype.test = function() {
    let idx = 0;
    let arr = this;
    return {
        has_next: function() {
            return idx < arr.length;
        },
        next: function() {
            if (this.has_next()) {
                return arr[idx++];
            } else {
                console.log("Element does not exist.");
                return null;
            }
        }
    };
};

const array_list = [6, 9, 10, 20, 33];
let it = array_list.test();

while (it.has_next()) {
    console.log(it.next());
}
