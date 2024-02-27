class MyArray {
    constructor(array) {
        this.idx = 0;
        this.array = array;
    }

    has_next() {
        return this.idx < this.array.length;
    }

    next() {
        if (this.has_next()) {
            return this.array[this.idx++];
        } else {
            console.log("Element does not exist.");
            return null;
        }
    }
}

const array_list = [6, 9, 10, 20, 33];
const it = new MyArray(array_list);

while (it.has_next()) {
    console.log(it.next());
}
