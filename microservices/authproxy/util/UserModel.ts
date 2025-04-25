import Mongoose, { Document } from 'mongoose';
import bcrypt from 'bcrypt';

export interface IUser extends Document {
    username: string;
    password: string;
}

const userSchema = new Mongoose.Schema<IUser>({
    username: { type: String, unique: true, required: true },
    password: { type: String, required: true }
});

userSchema.pre<IUser>('save', async function (next) {
    if (!this.isModified('password')) return next();

    try {
        const hashedPassword = await bcrypt.hash(this.password, 10);
        this.password = hashedPassword;
        next();
    } catch (err) {
        throw err;
    }
});

export const UserModel = Mongoose.model<IUser>('User', userSchema);
