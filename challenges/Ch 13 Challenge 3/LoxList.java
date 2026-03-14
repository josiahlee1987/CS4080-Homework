package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;

public class LoxList extends LoxInstance {
    private final List<Object> elements = new ArrayList<>();

    LoxList() {
        super(null);
    }

    @Override
    public Object get(Token name) {
        if (name.lexeme.equals("add")) {
            return new LoxCallable() {
                public int arity() { return 1; }
                public Object call(Interpreter i, List<Object> args) {
                    elements.add(args.get(0));
                    return null;
                }
            };
        }

        if (name.lexeme.equals("clear")) {
            return new LoxCallable() {
                @Override public int arity() { return 0; }
                @Override public Object call(Interpreter i, List<Object> args) {
                    elements.clear();
                    return null;
                }
            };
        }

        if (name.lexeme.equals("contains")) {
            return new LoxCallable() {
                @Override public int arity() { return 1; }
                @Override public Object call(Interpreter i, List<Object> args) {
                    return elements.contains(args.get(0));
                }
            };
        }

        if (name.lexeme.equals("get")) {
            return new LoxCallable() {
                public int arity() { return 1; }
                public Object call(Interpreter i, List<Object> args) {
                    int index = ((Double)args.get(0)).intValue();
                    return elements.get(index);
                }
            };
        }

        if (name.lexeme.equals("removeAt")) {
            return new LoxCallable() {
                @Override public int arity() { return 1; }
                @Override public Object call(Interpreter i, List<Object> args) {
                    int index = ((Double)args.get(0)).intValue();
                    return elements.remove(index);
                }
            };
        }

        if (name.lexeme.equals("size")) {
            return new LoxCallable() {
                public int arity() { return 0; }
                public Object call(Interpreter i, List<Object> args) {
                    return (double)elements.size();
                }
            };
        }

        throw new RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
    }
}