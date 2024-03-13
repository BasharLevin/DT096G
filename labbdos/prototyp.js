
class array{
    constructor(my_array, iter) {
        this.iter = iter;
        this.my_array = my_array;
    }
    has_next = function(){
        return this.iter < this.my_array.length;
    }
    next = function(){
        return this.my_array[this.iter++];
    }
}
Array.prototype.iter = function () {
    const it = new array(this, 0)
    while (it.has_next()) {
        console.log(it.next());
    }
    arr = [1, 69, 420, 666];
    arr.iter();
}